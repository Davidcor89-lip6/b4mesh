#include "b4mesh_p.hpp"


//Constructor - Global variables initialization
B4Mesh::B4Mesh(node* node, boost::asio::io_service& io_service, short port, std::string myIP)
    : node_(node),
      time_start(std::chrono::steady_clock::now()),
      io_service_(io_service),
      timer_generateT(io_service,std::chrono::steady_clock::now()),
      dist(300,600),  // payload size
      dist_exp(rng, boost::exponential_distribution<>(0.5)), // mean = 2
      mIP_(myIP)
{

    missing_parents_list = std::vector<pair<string, std::string>> ();
    pending_transactions = std::map<std::string, Transaction>();
    groupId = string(32,0);

    /* Variables for the blockchain performances */
    blocktxsSize = 5; // 130 Number of txs in mempool to creat a block.
    sizemempool = 500; //Limit size of the mempool in num of txs
    numTxsG = 0;  //Num of txs generated
    lostTrans = 0;
	lostPacket = 0;
    blockgraph_file = std::vector<std::pair<int, std::pair <int, int>>> ();


    //lancement transaction
    if ( node_->consensus_.AmILeader() )
    {
        std::cout << "i m leader " << std::endl;
        timer_generateT.expires_from_now(std::chrono::seconds(5));
        timer_generateT.async_wait(boost::bind(&B4Mesh::timer_generateT_fct, this, boost::asio::placeholders::error));
    }

}

B4Mesh::~B4Mesh(){
}


//  ************* PACKET RELATED METHODS ********************************
void B4Mesh::ReceivePacket(std::string packet, std::string ip)
{
    std::cout << "ReceivePacket: size " << packet.size() << std::endl; 
    try
    {
        ApplicationPacket p(packet);
        //std::cout << "message " << p << std::endl;

        // ------------ TRANSACTION TREATMENT ------------------ 
        if (p.GetService() == ApplicationPacket::TRANSACTION){
            Transaction t(p.GetPayload());
            std::cout << " Received a new transaction packet : " << t << " from " <<  ip << " with hash " << t.GetHash() << std::endl;
            TransactionsTreatment(t);
        }
        // ------------ BLOCK TREATMENT ------------------
        else if (p.GetService() == ApplicationPacket::BLOCK){
            Block b(p.GetPayload());
            std::cout << "Received a new block : " << b << " from " << ip << " with hash " << b.GetHash() << std::endl;
            BlockTreatment(b);
        }
        // ------------ REQUEST_BLOCK TREATMENT ------------------ 
        else if (p.GetService() == ApplicationPacket::REQUEST_BLOCK){
            string req_block = p.GetPayload();
            std::cout << " REQUEST_BLOCK: looking for block: " << req_block << std::endl;;
            //SendParentBlock(req_block, ip);
        }
        else{
            std::cout << " Packet type unsupported" << std::endl;
        }
  
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        std::cout << "Packet lost !!!!!!!!!" << std::endl;
		lostPacket++;
        return;
    }
}


// ************** GENERATION DES TRANSACTIONS *****************************
void B4Mesh::GenerateTransactions(){

    numTxsG += 1;

    /* Génération des variable aleatoires pour definir la taille de la transaction */
    int size_payload = dist(rng);

    Transaction t;
    t.SetPayload(string(size_payload, 'A'));
    t.SetTimestamp(getSeconds());

    std::cout << " Node: " << " size payload " << size_payload << std::endl;

    SendTransaction(t);
	// maybe need a sleep of 500ms here ? to avoid colliding transaction and block
	TransactionsTreatment(t);

    // restart transaction generation
    int interval = dist_exp()*1000 + 500; // add 500ms to avoid collidingif interval equal 0
    std::cout << " Node :" << " Random variable Expo: " << interval << "ms" <<std::endl;
    timer_generateT.expires_from_now(std::chrono::milliseconds(interval));
    timer_generateT.async_wait(boost::bind(&B4Mesh::timer_generateT_fct, this, boost::asio::placeholders::error));

}

void B4Mesh::SendTransaction(Transaction t){

    std::cout << "sending t "<< t << std::endl;
    string serie = t.Serialize();
    ApplicationPacket packet(ApplicationPacket::TRANSACTION, serie);

    node_->BroadcastPacket(packet);
}



//  ******* TRANSACTION RELATED METHODS **************** */
void B4Mesh::TransactionsTreatment(Transaction t)
{
    if (!IsTxInMempool(t)){
        std::cout << "Transaction not in mempool. " << std::endl;
        if (!IsTxInBlockGraph(t)){
            std::cout << "Transaction not in Blockgraph. " << std::endl;
            if (pending_transactions.size() <= sizemempool){
                std::cout << "Adding transaction in mempool... " << std::endl;
                pending_transactions[t.GetHash()] = t;
            } else { // No space in mempool.
                std::cout << "Transaction's Mempool is full" << std::endl;
                std::cout << "Dumping transaction..." << std::endl;
                lostTrans++;
            }
        } else { // Transaction already in blockgraph
            std::cout << "Transaction already present in Blockgraph\n Dumping transaction ...  " << std::endl;
        }
    } else { // Transaction already in mempool
        std::cout << "Transaction already present in Memepool\n Dumping transaction ... " << std::endl;
    }
    // If leader and enough transactions in mempool. Then create block.
    if(node_->consensus_.AmILeader() == true && TestPendingTxs() == true){
        GenerateBlocks();
    }
}

bool B4Mesh::IsTxInMempool (Transaction t)
{
    if(pending_transactions.count(t.GetHash()) > 0){
        return true;
    }
    else {
        return false;
    }
}

bool B4Mesh::IsTxInBlockGraph (Transaction t)
{
    for (const auto bkg : blockgraph.GetBlocks()){
        for (auto& t2 : bkg.second.GetTransactions()){
            if(t2.GetHash() == t.GetHash()){
                return true;
            }
        }
    }
    return false;
}

// ****** BLOCK RELATED METHODS *********************
void B4Mesh::BlockTreatment(Block b)
{
    std::cout << " BlockTreatment: Block made by leader: " << b.GetLeader() << " with hash " << b.GetHash() << std::endl;
    // Checking if the block is already in BLOCKGRAPH
    if (!blockgraph.HasBlock(b)){
        // Checking if the block is a missing block
        if (IsBlockInMissingList(b.GetHash())){
            updateMissingList(b.GetHash());
        }
        //Check ancestors blocks
        if (!CheckBlockParents(b)){
            // One or more parents of this block are unknown
            // Getting unknown list of parents
            vector<string> unknown_parents = GetUnknownParents(b);
            std::string ip = node_->consensus_.GetIPFromId(b.GetLeader());
            // adding unknown parents to the list of missing blocks
            updateMissingList(unknown_parents, ip);
            // Check if block is merge block to start fast synchronization
            if(IsBlockMerge(b)){
                std::cout << "This block is a merge block." << std::endl;
                //std::cout << "Starting synchronization process..." << std::endl;
                //SyncNode(unknown_parents, ip); // fast sync
            }
        }
        // Adding block to blockgraph
        std::cout << " BlockTreatment: Adding the block "<< b.GetHash() << " to the blockgraph" << std::endl;
        blockgraph.AddBlock(b);
        // Updating Mempool
        UpdatingMempool(b);
    } else {
        // The block is already present in the local blockgraph
        std::cout << " BlockTreatment: Block " << b.GetHash() << " already present in blockgraph\n  Dumping block..." << std::endl;
    }
}

bool B4Mesh::CheckBlockParents(Block &b){
    vector<string> parents_of_block = b.GetParents();
    vector<string> blocks_in_node = vector<string>();
    // vector for performances
    pair<int,pair<int,int>> create_blockgraph = pair<int,pair<int,int>> ();

    std::cout << "CheckBlockParents: Checking parents for newBlock "<< b.GetHash() << " in node: " << node_->consensus_.GetId() << std::endl;
    for (auto &p : parents_of_block){
        std::cout << "CheckBlockParents: CheckBlockParents: Parent block: " << p << std::endl;
        // ------- Traces Proposes ------Adding block to blockgraph file----
        if (p == "01111111111111111111111111111111"){
            create_blockgraph = make_pair(0,
                                        make_pair(0,stoi(b.GetHash())));
            blockgraph_file.push_back(create_blockgraph);
        } else {
            create_blockgraph = make_pair(stoi(b.GetGroupId()),
                                        make_pair(stoi(p),stoi(b.GetHash())));
            blockgraph_file.push_back(create_blockgraph);
        }
        //-----------------------------------
    }

    for(auto &blk : blockgraph.GetBlocks()){
        blocks_in_node.push_back(blk.second.GetHash());
    }

    for (auto &parent : parents_of_block){
        if(std::find(blocks_in_node.begin(), blocks_in_node.end(), parent) != blocks_in_node.end()){
            std::cout << " CheckBlockParents: Parent known!" << std::endl;
        }
        else {
            std::cout << " CheckBlockParents: Parent not known!" << std::endl;
            return false;
        }
    }
    return true;
}

vector<string> B4Mesh::GetUnknownParents(Block &b)
{
    vector<string> v_p_blk = b.GetParents();
    vector<string> vn = vector<string>();
    vector<string> unknown_p = vector<string> ();

    for(auto &blk : blockgraph.GetBlocks()){
        vn.push_back(blk.second.GetHash());
    }

    for (auto parent : v_p_blk){
        if (find(vn.begin(), vn.end(), parent) != vn.end()){
            //there is element
        }
        else {
            unknown_p.push_back(parent);
        }
    }
    return unknown_p;
}

bool B4Mesh::IsBlockMerge(Block &b){
  vector<string> parents_of_block = b.GetParents();

  if(parents_of_block.size() > 1 ){
    return true;
  }
  else {
    return false;
  }
}

void B4Mesh::UpdatingMempool (Block &b)
{
    vector<Transaction> t_in_b = b.GetTransactions();

    for (auto &t : t_in_b){
        if (pending_transactions.count(t.GetHash()) > 0){
            std::cout << " UpdatingMempool: Transaction " << t.GetHash() << " founded" << std::endl;
            std::cout << " UpdatingMempool: Erasing transaction from Mempool... " << std::endl;
            pending_transactions.erase(t.GetHash());
        }
        else {
            std::cout << " UpdatingMempool:  I don't know this transaction " << t.GetHash() << std::endl;
        }
    }
}

/* Retrieves the blocks replicated in a majority of nodes from
 * the consensus module. */
/*void B4Mesh::GetBlocksFromConsensus(){
  if (!running) return;
  vector<Block> blocks;
  blocks = GetB4MeshRaft(node->GetId())->RetrieveBlocks();

  debug_suffix.str("");
  debug_suffix << " Get: " << blocks.size() << " blocks from c4m" << endl;
  debug(debug_suffix.str());
  // Events needs to be scheduled one after the other
  float toSchedule = 0;
  for(auto& b : blocks){
    float process_time = (b.GetSize() / pow(10,3)); // 1 kbytes/s
    toSchedule += process_time;
    p_b_t_t += process_time;
    Simulator::Schedule(MilliSeconds(toSchedule),
          &B4Mesh::BlockTreatment, this, b);
  }
}*/

/*void B4Mesh::SendParentBlock(string hash_p, Ipv4Address destAddr){
  Block block;

  if (blockgraph.HasBlock(hash_p)){
    debug_suffix.str("");
    debug_suffix << " SendParentBlock: Block " << hash_p << " founded!" << endl;
    debug(debug_suffix.str());
    block = blockgraph.GetBlock(hash_p);
  }
    // Case when block is not present...
    // is very unlikely since we already check that this node has this block.
  else {
    debug_suffix.str("");
    debug_suffix << " SendParentBlock: Block " << hash_p << " not founded!" << endl;
    debug(debug_suffix.str());
    return;
  }

  debug_suffix.str("");
  debug_suffix << " SendParentBlock: Sending block to node: " << destAddr << endl;
  debug(debug_suffix.str());
  ApplicationPacket packet(ApplicationPacket::BLOCK, block.Serialize());
  SendPacket(packet, destAddr);
  // Trace for block propagation delai
  TraceBlockCreate(GetIdFromIp(destAddr), Simulator::Now().GetSeconds(), stoi(block.GetHash()));
}*/

/*void B4Mesh::SendChildlessBlocks(Ipv4Address destAddr){
  vector<Block> childless_b_list = blockgraph.GetChildlessBlocks();
  string serie_hashes = "";


  for (auto chless : childless_b_list){
    string tmp_str = chless.GetHash(); //
    if (tmp_str != "01111111111111111111111111111111"){
      debug_suffix.str("");
      debug_suffix << " SendChildlessBlocks: Childless block founded : " << tmp_str;
      debug(debug_suffix.str());

      serie_hashes += tmp_str;
    }
  }

  if (serie_hashes.size() > 0 ){
    childlessblock_rep_hdr_t ch_rep;
    ch_rep.msg_type = CHILDLESSBLOCK_REP;
    string serie((char*)&ch_rep, sizeof(childlessblock_rep_hdr_t));
    serie += serie_hashes;
    ApplicationPacket pkt(ApplicationPacket::CHANGE_TOPO, serie);
    SendPacket(pkt, destAddr, false);
  } else {
    debug(" SendChildlessBlocks: No childless block founded!");
  }
}*/

/*void B4Mesh::CheckBranchesSync(const string& msg_payload, Ipv4Address destAddr){
  // Only executed by the leader
  string deserie = msg_payload;
  string tmp_hash = "";

  deserie = deserie.substr(sizeof(childlessblock_rep_hdr_t), deserie.size());

  vector<string> otherNode_childless = vector<string> ();
  vector<string> unknown_b_hash = vector<string> ();

  //deserialisation
  while ( deserie.size() > 0){
    tmp_hash = deserie.substr(0, 32);
    otherNode_childless.push_back(tmp_hash);
    deserie = deserie.substr(32, deserie.size());
  }

  for (auto &hash : otherNode_childless){
    debug(" CheckBranchesSync: This block is not a childless block of mine.");
    debug(" CheckBranchesSync: Checking if block is present in local blockgraph...");

    if(blockgraph.HasBlock(hash)){
        debug(" CheckBranchesSync: Block is present in local blockgraph...Ignoring block");
        continue;
    } else {
      debug(" CheckBranchesSync: Block is not present in blockgraph...Checking if block is present in blocks_to_sync");
      if (IsBlockInMissingList(hash)){
        debug(" CheckBranchesSync: Block already in waiting list. Ignoring block");
        continue;
      } else {
        debug(" CheckBranchesSync: Block not in waiting list. Adding block to waiting list ");
        missing_parents_list.push_back(make_pair(hash, destAddr));
        debug_suffix.str("");
        debug_suffix << " CheckBranchesSync: Asking for this block's " << hash << " chain " << endl;
        debug(debug_suffix.str());
        unknown_b_hash.push_back(hash);
      }
    }
  }

  if (unknown_b_hash.size() > 0){
    // Ask for block branch
    group_branch_hdr_t branch_req;
    branch_req.msg_type = GROUPBRANCH_REQ;

    string serie_unknown_b_h = "";

    for (auto &u_hash : unknown_b_hash){
      debug_suffix.str("");
      debug_suffix << " CheckBranchesSync: Asking for chain of block " << u_hash;
      debug(debug_suffix.str());
      serie_unknown_b_h += u_hash;
    }

    string serie((char*)&branch_req, sizeof(group_branch_hdr_t));
    serie += serie_unknown_b_h;
    ApplicationPacket pkt(ApplicationPacket::CHANGE_TOPO, serie);
    SendPacket(pkt, destAddr, false);

  }
  else {
    debug(" CheckBranchesSync: Blockgraph Up-to-date or synchronization already in process.");
  }
}*/

/*void B4Mesh::SendBranch4Sync(const string& msg_payload, Ipv4Address destAddr){
  string deserie = msg_payload;
  string block_hash = "";
  string gp_id = "";
  vector<string> block_hash_v = vector<string> ();
  vector<string> gp_id_v = vector<string> ();

  deserie = deserie.substr(sizeof(group_branch_hdr_t), deserie.size());

  // Extracting the block hashes in payload and putting them in vector block_hash_v
  while ( deserie.size() > 0){
    block_hash = deserie.substr(0, 32);
    if(find(block_hash_v.begin(), block_hash_v.end(), block_hash) != block_hash_v.end()){
    } else {
      block_hash_v.push_back(block_hash);
    }
    deserie = deserie.substr(32, deserie.size());
  }

   // for every block in block_hash_v
   //  getting the groupId of each block and filling the vector gp_id_v
   //  This vector has the GroupId of the blocks in the request.
   
  for (auto &b_h : block_hash_v){
    gp_id = blockgraph.GetGroupId(b_h);
    if (find(gp_id_v.begin(), gp_id_v.end(), gp_id) != gp_id_v.end()){
      //GroupId already existent "
    } else {
      gp_id_v.push_back(gp_id);
    }
  }

  // Sending blocks with the same groupId to node
  for (auto g_id : gp_id_v){
    vector<Block> blocks_group = blockgraph.GetBlocksFromGroup(g_id);
    for (auto b_grp : blocks_group){
      debug_suffix.str("");
      debug_suffix << " SendBranch4Sync: Sending block hash " << b_grp.GetHash() << " to node " << destAddr << endl;
      debug(debug_suffix.str());
      ApplicationPacket packet(ApplicationPacket::BLOCK, b_grp.Serialize());
      SendPacket(packet, destAddr);
      // Trace for block propagation delai
      TraceBlockCreate(GetIdFromIp(destAddr), Simulator::Now().GetSeconds(), stoi(b_grp.GetHash()));
    }
  }
}*/

// ********* Missing Parents block list METHODS **************
bool B4Mesh::IsBlockInMissingList(string b_hash){

  for (auto &pair_m_b : missing_parents_list){
    if (b_hash == pair_m_b.first){
      return true;
    }
  }
  return false;
}

void B4Mesh::updateMissingList(string b_hash){
  // Checking that the new block is not a missing blocks
  for (auto pair_m_b = missing_parents_list.begin(); pair_m_b != missing_parents_list.end(); pair_m_b++){
    if(pair_m_b->first == b_hash){
      std::cout << "updateMissingList2: new Block " << b_hash << " is a missing parent" << std::endl;
      std::cout << "updateMissingList2: Erasing block hash from missing_parents_list" << std::endl;
      missing_parents_list.erase(pair_m_b);
      return;
    }
  }
  std::cout << "updateMissingList2: Not a missing block" << std::endl;
}

void B4Mesh::updateMissingList(vector<string> unknown_p, std::string ip)
{
  // Updating missing_parents_list
  bool flag = false;

  for (auto &missing_p : unknown_p){
    flag = false;
    for (auto &pair_m_b : missing_parents_list){
      if (missing_p == pair_m_b.first){
        std::cout << "updateMissingList: Parent " << missing_p << " already in list" << std::endl;
        flag  = true;
      }
    }
    if (flag == false){
      std::cout << "updateMissingList: Parent " << missing_p << " not founded in list" << std::endl;
      std::cout << "updateMissingList: Adding missing parent to list of missing blocks" << std::endl;
      missing_parents_list.push_back(make_pair(missing_p, ip));
    }
  }
}

void B4Mesh::Ask4MissingBlocks()
{
    if (missing_parents_list.size() > 0)
    {
        std::cout << " Ask4MissingBlocks: List of missing parents" << std::endl;
        for (auto &a : missing_parents_list){
            std::cout << " block: " << a.first << std::endl;
        }
        for (auto &pair_m_b : missing_parents_list){
            std::cout << "Ask4MissingBlocks: asking for block: " << pair_m_b.first << std::endl;
            string serie = pair_m_b.first;
            ApplicationPacket packet(ApplicationPacket::REQUEST_BLOCK, serie);
            node_->SendPacket(pair_m_b.second, packet);
        }
    } else {
        std::cout << " Ask4MissingBlocks: No blocks in waiting list" << std::endl;
    }
}


// ************** GENERATION DES BLOCKS *****************************
bool B4Mesh::TestPendingTxs(){
    // Only leader execute this function

    if (pending_transactions.size() > blocktxsSize){
        std::cout << " TestPendingTxs: Enough txs to create a block." << std::endl;;
        return true;
    }
    else {
        std::cout << " Not enough transactions to form a block" << std::endl;;
        return false;
    }
}


void B4Mesh::GenerateBlocks(){
	// Initialize the block
	Block block;
	block.SetLeader(node_->consensus_.GetId());
	block.SetGroupId(node_->consensus_.GetGroupId());

	//filling the block with transactions
	std::cout << " GenerateBlock: Size of pending_transactions: " << pending_transactions.size() << std::endl;
	vector<Transaction> transactions;
	for (auto &t : pending_transactions){
		transactions.push_back(t.second);
		if (transactions.size() > blocktxsSize - 1){
			break;
		}
	}
	block.SetTransactions(transactions);

	// Remove transactions from the list of pending transactions
	// Transactions from the leader need to be removed Here
	// and not at the block reception because the leader's Mempool size
	// is the critirial to create blocks. If transactions are not removed
	// blocks are going to be created with repeated txs
	
	for (auto &t : transactions)
		pending_transactions.erase(t.GetHash());


	// Getting Parents of the futur Block
	vector<string> p_block = vector<string> ();

	// Setting parents' block
	for (auto& tb : blockgraph.GetChildlessBlocks()){
		std::cout << " GenerateBlock: Hash parent is: " << tb.GetHash() << std::endl;
		p_block.push_back(tb.GetHash());
	}
	block.SetParents(p_block);

	int index = -1;
	for (auto &p : p_block){
		index = max(index, blockgraph.GetBlock(p).GetIndex());
	}
	block.SetIndex(index+1);
	block.SetTimestamp(getSeconds());

	if(p_block.size() > 1){
		std::cout << "Sending a MERGE BLOCK" << std::endl;
	}

	BlockTreatment(block);
	std::cout << " Block size is:   " << block.GetSize() <<  std::endl;
	SendBlockToConsensus(block);

}

void B4Mesh::SendBlockToConsensus(Block b)
{
	// Using c4m to publish the block
	std::cout << " SendBlockToConsensus:   " << b.GetHash() << std::endl;
	std::cout << "sending b "<< b << std::endl;
	string serie = b.Serialize();
	ApplicationPacket packet(ApplicationPacket::BLOCK, serie);
	
	node_->BroadcastPacket(packet);
}

// ************** CALCUL AND GROUP DETECTION METHODS ************************
/*void B4Mesh::ReceiveNewTopologyInfo(pair<string, vector<pair<int, Ipv4Address>>> new_group){
	groupId = new_group.first;
	group = new_group.second;
}*/

/*void B4Mesh::StartMerge(){
  if (running == false){
    return;
  }

  if (GetB4MeshRaft(node->GetId())->IsLeader() == false)
    return;

  debug(" Starting Merge at Node: ");
  debug_suffix.str("");
  debug_suffix << " StartMerge: Leader node: " << node->GetId()
               << " starting the leader synchronization process..." << endl;
  debug(debug_suffix.str());
  CheckBlockgraphSync();
}*/

/*void B4Mesh::CheckBlockgraphSync(){
  debug(" CheckBlockgraphSync: In leader synch process ");
  childlessblock_req_hdr_t ch_req;
  ch_req.msg_type = CHILDLESSBLOCK_REQ;

  ApplicationPacket packet(ApplicationPacket::CHANGE_TOPO,
      string((char*)&ch_req, sizeof(ch_req)));

  vector<Ipv4Address> groupDestination = vector<Ipv4Address> ();
  for (auto& dest : group){
    groupDestination.push_back(dest.second);
  }
  BroadcastPacket(packet, groupDestination);
}*/
// *************** TRACES AND PERFORMANCES ***********************************

int B4Mesh::SizeMempoolBytes (){
    int ret = 0;
    for (auto &t : pending_transactions){
    ret += t.second.GetSize();
    }
    return ret;
}

void B4Mesh::GenerateResults()
{
    std::cout << " GenerateResults " << std::endl;

    std::cout << "\nB4mesh: Stop B4Mesh on node : " << node_->consensus_.GetId() << std::endl;
    std::cout << "B4mesh: Total number of blocks : " << blockgraph.GetBlocksCount() << std::endl;
    std::cout << "B4Mesh: Taille moyenne d'un bloc (bytes): " << blockgraph.GetByteSize()/blockgraph.GetBlocksCount() << std::endl;
    std::cout << "B4Mesh: Blocks restant is missing_parents_list: " << missing_parents_list.size() << std::endl;
    for (auto &b : missing_parents_list){
    	std::cout << "B4Mesh: - Block #: " << dump(b.first.data(), 10) << " is missing " << std::endl;
    }
    std::cout << "B4mesh: Total number of transactions in BG : " << blockgraph.GetTxsCount() << std::endl;
    std::cout << "B4Mesh: Moyenne de transactions par block dans Blockgraph: " << blockgraph.MeanTxPerBlock() << std::endl;
    std::cout << "B4Mesh: Total size of transactions (bytes): " << blockgraph.GetTxsByteSize() << std::endl;
    std::cout << "B4Mesh: Transactions restant dans le mempool: " << pending_transactions.size() << std::endl;
    std::cout << "B4Mesh: Transactions lost due to space: " << lostTrans << std::endl;
	std::cout << "B4Mesh: Packets lost (transaction or block) due to network jamming: " << lostPacket << std::endl;
    std::cout << "B4Mesh: Num of transaction generated in this node: " << numTxsG << std::endl;
    std::cout << "B4Mesh: Total Bytes : " << blockgraph.GetByteSize() << "\n" << std::endl;
    
}
