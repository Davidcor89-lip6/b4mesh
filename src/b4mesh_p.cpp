#include "b4mesh_p.hpp"


//Constructor - Global variables initialization
B4Mesh::B4Mesh(node* node, boost::asio::io_service& io_service, short port, std::string myIP)
    : node_(node),
	  mIP_(myIP),
      time_start(std::chrono::steady_clock::now()),
      io_service_(io_service),
      dist(300,600),  // payload size
      dist_exp(rng, boost::exponential_distribution<>(0.5)), // mean = 2
	  timer_generateT(io_service,std::chrono::steady_clock::now()),
	  timer_recurrentTask(io_service,std::chrono::steady_clock::now())
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

    //recurrent task
    timer_recurrentTask.expires_from_now(std::chrono::seconds(RECCURENT_TIMER));
	timer_recurrentTask.async_wait(boost::bind(&B4Mesh::timer_recurrentTask_fct, this, boost::asio::placeholders::error));

    //lancement transaction
    //if ( node_->consensus_.AmILeader() )
    {
        std::cout << "i m leader " << std::endl;
        timer_generateT.expires_from_now(std::chrono::seconds(5));
        timer_generateT.async_wait(boost::bind(&B4Mesh::timer_generateT_fct, this, boost::asio::placeholders::error));
    }

}

B4Mesh::~B4Mesh(){
}

// ************** Recurrent Task **************************************
void B4Mesh::timer_recurrentTask_fct (const boost::system::error_code& /*e*/) 
{
	Ask4MissingBlocks();
    timer_recurrentTask.expires_from_now(std::chrono::seconds(RECCURENT_TIMER));
	timer_recurrentTask.async_wait(boost::bind(&B4Mesh::timer_recurrentTask_fct, this, boost::asio::placeholders::error));

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
            SendParentBlock(req_block, ip);
        }
		/* ------------ CHANGE_TOPO TREATMENT ------------------ */
		else if (p.GetService() == ApplicationPacket::CHANGE_TOPO){
			int message_type = ExtractMessageType(p.GetPayload());
			if ( message_type == CHILDLESSBLOCK_REQ){ //TODO needed ?
				// send to leader your childless blocks.
				SendChildlessBlocks(ip);
			}
			else if (message_type == CHILDLESSBLOCK_REP){ //TODO needed ?
				// Reply only received by leaders -
				CheckBranchesSync(p.GetPayload(), ip);
			}
			else if (message_type == GROUPBRANCH_REQ){ //TODO needed ?
				// All nodes can enter here
				SendBranch4Sync(p.GetPayload(), ip);
			}
			else {
				std::cout << " Packet CHANGE_TOPO type unsupported" << std::endl;
			}
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
	  usleep(200); // TODO : maybe need a sleep of 500ms here ? to avoid colliding transaction and block 
	  TransactionsTreatment(t);

    // restart transaction generation
    int interval = dist_exp()*1000 + 500; // TODO : added 500ms to avoid colliding if interval equal 0
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

int B4Mesh::ExtractMessageType(const string& msg_payload){
  int ret = *((int*)msg_payload.data());
  return ret;
}

//  ******* TRANSACTION RELATED METHODS **************** */
void B4Mesh::TransactionsTreatment(Transaction t)
{
    if (!IsTxInMempool(t)){
        std::cout << "Transaction not in mempool. " << std::endl;
        if (!IsTxInBlockGraph(t)){
            std::cout << "Transaction not in Blockgraph. " << std::endl;
            if (pending_transactions.size() <= sizemempool){
                std::cout << "Adding transaction in mempool... (" << t.GetHash() << ")" << std::endl;
                pending_transactions[t.GetHash()] = t;
                //DumpMempool();
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

void B4Mesh::DumpMempool (void)
{
    std::cout << "Dump MemPool " << std::endl;
    for(auto it = pending_transactions.begin(); it != pending_transactions.end(); ++it)
    {
      std::cout << it->first << std::endl; 
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
                SyncNode(unknown_parents, ip); // fast sync
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

void B4Mesh::SyncNode(vector<string> unknown_p, std::string ip){

  if (unknown_p.size() > 0){
    // Ask for block branch
    group_branch_hdr_t branch_req;
    branch_req.msg_type = GROUPBRANCH_REQ;

    std::string serie_unknown_b_h = "";

    for (auto &u_hash : unknown_p){
      std::cout << " SyncNode: Asking for block's " << u_hash << " branch to "
                   << ip << std::endl;
      serie_unknown_b_h += u_hash;
    }

    std::string serie((char*)&branch_req, sizeof(group_branch_hdr_t));
    serie += serie_unknown_b_h;
    ApplicationPacket pkt(ApplicationPacket::CHANGE_TOPO, serie);
    node_->SendPacket(ip, pkt);
  }
}

void B4Mesh::UpdatingMempool (Block &b)
{
    vector<Transaction> t_in_b = b.GetTransactions();

    //DumpMempool();

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

void B4Mesh::SendParentBlock(string hash_p, std::string destAddr){
  Block block;

  if (blockgraph.HasBlock(hash_p)){
    std::cout << " SendParentBlock: Block " << hash_p << " founded!" << std::endl;
    block = blockgraph.GetBlock(hash_p);
  }
    // Case when block is not present...
    // is very unlikely since we already check that this node has this block.
  else {
    std::cout << " SendParentBlock: Block " << hash_p << " not founded!" << std::endl;
    return;
  }

  std::cout << " SendParentBlock: Sending block to node: " << destAddr << std::endl;
  ApplicationPacket packet(ApplicationPacket::BLOCK, block.Serialize());
  node_->SendPacket(destAddr, packet);
}

void B4Mesh::SendChildlessBlocks(std::string destAddr){
  std::vector<Block> childless_b_list = blockgraph.GetChildlessBlocks();
  std::string serie_hashes = "";


  for (auto chless : childless_b_list){
    string tmp_str = chless.GetHash(); //
    if (tmp_str != "01111111111111111111111111111111"){
      std::cout << " SendChildlessBlocks: Childless block founded : " << tmp_str << std::endl;

      serie_hashes += tmp_str;
    }
  }

  if (serie_hashes.size() > 0 ){
    childlessblock_rep_hdr_t ch_rep;
    ch_rep.msg_type = CHILDLESSBLOCK_REP;
    string serie((char*)&ch_rep, sizeof(childlessblock_rep_hdr_t));
    serie += serie_hashes;
    ApplicationPacket pkt(ApplicationPacket::CHANGE_TOPO, serie);
    node_->SendPacket(destAddr, pkt);
  } else {
    std::cout << " SendChildlessBlocks: No childless block founded!"<< std::endl;
  }
}

void B4Mesh::CheckBranchesSync(const std::string& msg_payload, std::string destAddr){
  // Only executed by the leader
  std::string deserie = msg_payload;
  std::string tmp_hash = "";

  deserie = deserie.substr(sizeof(childlessblock_rep_hdr_t), deserie.size());

  std::vector<std::string> otherNode_childless = std::vector<std::string> ();
  std::vector<std::string> unknown_b_hash = std::vector<std::string> ();

  //deserialisation
  while ( deserie.size() > 0){
    tmp_hash = deserie.substr(0, 32);
    otherNode_childless.push_back(tmp_hash);
    deserie = deserie.substr(32, deserie.size());
  }

  for (auto &hash : otherNode_childless){
    std::cout << " CheckBranchesSync: This block is not a childless block of mine."<< std::endl;
    std::cout <<" CheckBranchesSync: Checking if block is present in local blockgraph..."<< std::endl;

    if(blockgraph.HasBlock(hash)){
        std::cout <<" CheckBranchesSync: Block is present in local blockgraph...Ignoring block"<< std::endl;
        continue;
    } else {
      std::cout <<" CheckBranchesSync: Block is not present in blockgraph...Checking if block is present in blocks_to_sync"<< std::endl;
      if (IsBlockInMissingList(hash)){
        std::cout <<" CheckBranchesSync: Block already in waiting list. Ignoring block"<< std::endl;
        continue;
      } else {
        std::cout <<" CheckBranchesSync: Block not in waiting list. Adding block to waiting list "<< std::endl;
        missing_parents_list.push_back(make_pair(hash, destAddr));
        std::cout << " CheckBranchesSync: Asking for this block's " << hash << " chain " << std::endl;
        unknown_b_hash.push_back(hash);
      }
    }
  }

  if (unknown_b_hash.size() > 0){
    // Ask for block branch
    group_branch_hdr_t branch_req;
    branch_req.msg_type = GROUPBRANCH_REQ;

    std::string serie_unknown_b_h = "";

    for (auto &u_hash : unknown_b_hash){
      std::cout <<  " CheckBranchesSync: Asking for chain of block " << u_hash << std::endl;;
      serie_unknown_b_h += u_hash;
    }

    std::string serie((char*)&branch_req, sizeof(group_branch_hdr_t));
    serie += serie_unknown_b_h;
    ApplicationPacket pkt(ApplicationPacket::CHANGE_TOPO, serie);
    node_->SendPacket(destAddr, pkt);

  }
  else {
    std::cout << " CheckBranchesSync: Blockgraph Up-to-date or synchronization already in process."<< std::endl;
  }
}

void B4Mesh::SendBranch4Sync(const std::string& msg_payload, std::string destAddr){
  std::string deserie = msg_payload;
  std::string block_hash = "";
  std::string gp_id = "";
  std::vector<std::string> block_hash_v = std::vector<std::string> ();
  std::vector<std::string> gp_id_v = std::vector<std::string> ();

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
    std::vector<Block> blocks_group = blockgraph.GetBlocksFromGroup(g_id);
    for (auto b_grp : blocks_group){
      std::cout << " SendBranch4Sync: Sending block hash " << b_grp.GetHash() << " to node " << destAddr << std::endl;
      ApplicationPacket packet(ApplicationPacket::BLOCK, b_grp.Serialize());
      node_->SendPacket(destAddr, packet);
    }
  }
}

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

	/*// Remove transactions from the list of pending transactions
	// Transactions from the leader need to be removed Here
	// and not at the block reception because the leader's Mempool size
	// is the critirial to create blocks. If transactions are not removed
	// blocks are going to be created with repeated txs
	
	for (auto &t : transactions)
		pending_transactions.erase(t.GetHash());*/ // in the case of the POC, is not necessary ???


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

void B4Mesh::StartMerge(){

  if (node_->consensus_.AmILeader() == false)
    return;

  std::cout << " StartMerge: Leader node: " << node_->consensus_.GetId() << std::endl;
  std::cout << " starting the leader synchronization process..." << std::endl; 
  CheckBlockgraphSync();
}

void B4Mesh::CheckBlockgraphSync(){
  std::cout << " CheckBlockgraphSync: In leader synch process "<< std::endl;
  childlessblock_req_hdr_t ch_req;
  ch_req.msg_type = CHILDLESSBLOCK_REQ;

  ApplicationPacket packet(ApplicationPacket::CHANGE_TOPO,
      std::string((char*)&ch_req, sizeof(ch_req)));

  node_->BroadcastPacket(packet);
}
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

	// ---The blockgraph into file. ----
	ofstream output_file;
	char filename[50];
	sprintf(filename, "blockgraph-%d.txt", node_->consensus_.GetId());
	output_file.open(filename, ios::out);
	output_file << "#BlockGroup" << " " << "ParentBlock" << " " << "BlockHash" << std::endl;
	for (auto &it : blockgraph_file){
		output_file << it.first << " " << it.second.first << " " << it.second.second << std::endl;
	}
	output_file.close();

	// ---Dump the rest of the mempool into file. ----
	ofstream output_file2;
	char filename2[50];
	sprintf(filename2, "mempool-%d.txt", node_->consensus_.GetId());
	output_file2.open(filename2, ios::out);
	output_file2 << "#mempool" << std::endl;
	for(auto &it : pending_transactions)
    {
      output_file2 << it.second << std::endl; 
    }
	output_file2.close();
    
}
