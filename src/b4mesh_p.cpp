#include "b4mesh_p.hpp"

//Constructor - Global variables initialization
B4Mesh::B4Mesh(node* node, boost::asio::io_context& io_context, short port, std::string myIP, bool geneTrans)
  : visuBlock(LIVEBLOCK_FILE),
  visuMemPool(LIVEMEMPOOL_FILE),
  node_(node),
  mIP_(myIP),
  time_start(std::chrono::steady_clock::now()),
  io_context_(io_context),
  rng((unsigned int)time(NULL)+getpid()),  
  dist(PAYLOAD_MIN,PAYLOAD_MAX),  // payload size
  dist_exp(rng, boost::exponential_distribution<>(LAMBDA_DIST)), 
  timer_generateT(io_context,std::chrono::steady_clock::now()),
  timer_recurrentTask(io_context,std::chrono::steady_clock::now()),
  timer_childless(io_context,std::chrono::steady_clock::now())
{
  /* Group Management System Variables Init */
  groupId = string(32,0);
  /* Blockgraph Protocol Variables Init */
  missing_block_list = std::vector<pair<string, std::string>> ();
  missing_childless = std::vector<std::string> ();
  waiting_list = std::map<std::string, Block>();
  pending_transactions = std::map<std::string, Transaction>();
	recover_branch = std::multimap<int, std::string> ();
	mergeBlock = false;
  createBlock = true;
	lastBlock_creation_time = 0;

  /* Variables for the blockchain performances */
  numTxsG = 0;  //Num of txs generated
	numRTxsG = 0;
  lostTrans = 0;
	lostPacket = 0;
	numDumpingBlock = 0;
  numDumpingTxs = 0;
	total_missingBlockTime = 0;
  count_missingBlock = 0;
	total_waitingBlockTime = 0;
  count_waitingBlock = 0;
	total_pendingTxTime = 0;
  count_pendingTx = 0;
  blockgraph_file = std::vector<std::pair<int, std::pair <int, int>>> ();



  //recurrent task
  timer_recurrentTask.expires_from_now(std::chrono::seconds(RECCURENT_TIMER));
	timer_recurrentTask.async_wait(boost::bind(&B4Mesh::timer_recurrentTask_fct, this, boost::asio::placeholders::error));

  //lancement transaction
  if ( geneTrans )
  {
      timer_generateT.expires_from_now(std::chrono::seconds(WAIT_FOR_FIRST_TRANSACTION));
      timer_generateT.async_wait(boost::bind(&B4Mesh::timer_generateT_fct, this, boost::asio::placeholders::error));
  }
}

B4Mesh::~B4Mesh(){
}

void B4Mesh::setCreateBlock (bool cb)
{
  createBlock = cb;
}

// ************** Recurrent Task **************************************
void B4Mesh::timer_recurrentTask_fct (const boost::system::error_code& /*e*/) 
{
	Ask4MissingBlocks();
  RetransmitTransactions();

  timer_recurrentTask.expires_from_now(std::chrono::seconds(RECCURENT_TIMER));
	timer_recurrentTask.async_wait(boost::bind(&B4Mesh::timer_recurrentTask_fct, this, boost::asio::placeholders::error));

}


//  ************* PACKET RELATED METHODS ********************************
int B4Mesh::ExtractMessageType(const string& msg_payload){
  int ret = *((int*)msg_payload.data());
  return ret;
}

void B4Mesh::ReceivePacket(std::string packet, std::string ip)
{
    DEBUG << "ReceivePacket: size " << packet.size() << " from " << ip << std::endl; 
    try
    {
      ApplicationPacket p(packet, packet.size());

      // ------------ TRANSACTION PACKET ------------------ 
      if (p.GetService() == ApplicationPacket::TRANSACTION){
          Transaction t(p.GetPayload());
          DEBUG << BOLDCYAN << " Received a new transaction packet : " << t << " from " <<  ip << " with hash " << t.GetHash() << RESET << std::endl;
          TransactionsTreatment(t);
      }
      // ------------ BLOCK PACKET ------------------
      else if (p.GetService() == ApplicationPacket::BLOCK){
          DEBUG << GREEN << "BLOCK PACKET" << std::endl;
          Block b(p.GetPayload());
          DEBUG << GREEN << "Received a new block : " << b << " from " << ip << " with hash " << b.GetHash() << RESET << std::endl;
          BlockTreatment(b);
      }
      // ------------ REQUEST_BLOCK PACKET ------------------ 
      else if (p.GetService() == ApplicationPacket::REQUEST_BLOCK){
          string req_block = p.GetPayload();
          DEBUG << BLUE << " REQUEST_BLOCK: looking for block: " << req_block << RESET << std::endl;
          SendBlockto(req_block, ip);
      }
		  /* ------------ CHANGE_TOPO PACKET ------------------ */
      else if (p.GetService() == ApplicationPacket::CHANGE_TOPO){
        int message_type = ExtractMessageType(p.GetPayload());
        /* ------------ CHILDLESSBLOCK_REQ PACKET ------------------ */
        if ( message_type == CHILDLESSBLOCK_REQ){
          //Upon reception: send to leader childless blocks.
          DEBUG << RED << " CHILDLESSBLOCK_REQ: " << ip << RESET << std::endl;
          SendChildlessBlocks(ip);
        }
        /* ------------ CHILDLESSBLOCK_REP PACKET ------------------ */
        else if (message_type == CHILDLESSBLOCK_REP){
          // Upon reception: Leader execute this functin -
          DEBUG << BOLDYELLOW << " CHILDLESSBLOCK_REP: " << ip << RESET << std::endl;
          ProcessChildlessResponse(p.GetPayload(), ip);
        }
        /* ------------ GROUPBRANCH_REQ PACKET ------------------ */
        else if (message_type == GROUPBRANCH_REQ){
          // Send a branch of the blockhraph to a node
          DEBUG << MAGENTA << " GROUPBRANCH_REQ: " << ip << RESET << std::endl;
          SendBranch4Sync(p.GetPayload(), ip);
        }
        else {
          DEBUG << " Packet CHANGE_TOPO type unsupported" << std::endl;
        }
      }
        else{
            DEBUG << RED << " Packet type unsupported or faulty!" << RESET << std::endl;
			lostPacket++;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        DEBUG << "Packet lost !!!!!!!!!" << std::endl;
		lostPacket++;
        return;
    }
}

// ************** GENERATION DES TRANSACTIONS *****************************
void B4Mesh::GenerateTransactions(){
    
    numTxsG += 1;

    /* Génération des variable aleatoires pour definir la taille de la transaction */
    int size_payload = dist(rng);

    RegisterTransaction(string(size_payload, 'A'+node_->consensus_.GetId()));

    // restart transaction generation
    int interval = dist_exp()*1000; 
    DEBUG << " -> Next Transaction in : " << interval << "ms" <<std::endl;
    timer_generateT.expires_from_now(std::chrono::milliseconds(interval));
    timer_generateT.async_wait(boost::bind(&B4Mesh::timer_generateT_fct, this, boost::asio::placeholders::error));
}

void B4Mesh::RegisterTransaction(std::string payload){
    Transaction t;
    t.SetPayload(payload);
    t.SetTimestamp(getSeconds());
    DEBUG << " Node: " << node_->consensus_.GetId() << " size payload " << payload.size() << std::endl;
    SendTransaction(t);
    TransactionsTreatment(t);
}

void B4Mesh::SendTransaction(Transaction t){

	DEBUG << BOLDCYAN << " sending new transaction packet : " << t << " with hash " << t.GetHash() << RESET << std::endl;
  string serie = t.Serialize();
  ApplicationPacket packet(ApplicationPacket::TRANSACTION, serie);

  node_->BroadcastPacket(packet, false);
}

//  ******* TRANSACTION RELATED METHODS **************** */
void B4Mesh::TransactionsTreatment(Transaction t)
{
  auto t1 = std::chrono::high_resolution_clock::now();
  if (!IsTxInMempool(t)){
  //    DEBUG << "Transaction not in mempool. " << std::endl;
      if (!blockgraph.IsTxInBG(t)){
    //      DEBUG << "Transaction not in Blockgraph. " << std::endl;
          if (IsSpaceInMempool()){
      //        DEBUG << "Adding transaction in mempool... (" << t.GetHash() << ")" << std::endl;
              pending_transactions[t.GetHash()] = t;
              // trace purpose
              pending_transactions_time[t.GetHash()] = getSeconds();
          } else { // No space in mempool.
          //     DEBUG << "Transaction's Mempool is full" << std::endl;
          //     DEBUG << "Dumping transaction..." << std::endl;
              lostTrans++;
          }
      } else { // Transaction already in blockgraph
      //    DEBUG << "Transaction already present in Blockgraph\n Dumping transaction ...  " << std::endl;
          numDumpingTxs++;
      }
  } else { // Transaction already in mempool
    //  DEBUG << "Transaction already present in Memepool\n Dumping transaction ... " << std::endl;
      numDumpingTxs++;
  }

  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> time_treatment = t2 - t1;
  tx_treatment_time.push_back(time_treatment.count());
  total_treatment_tx_t += time_treatment;

  // If leader and enough transactions in mempool. Then create block.
  if(node_->consensus_.AmILeader() == true){
    if (TestPendingTxs() == true && createBlock == true){     
      GenerateBlocks();
    }
  }
}

void B4Mesh::RetransmitTransactions(){
  // Retransmission of transactions to be sure that old transactions are register
	if (pending_transactions.size() > 0)
	{
		for(auto &mem_i: pending_transactions)
		{
		//	Transaction t = mem_i.second;
			if ( mem_i.second.GetTimestamp() + T_RETRANS < getSeconds() )
			{
				DEBUG << YELLOW << "--> Retransmission transation" << RESET << std::endl;
				numRTxsG ++;
				SendTransaction(mem_i.second);
			}
		}
	}
  // For trace purpose
	visuMemPool << pending_transactions.size() << " " << (float)(pending_transactions.size())/(float)(SIZE_MEMPOOL) << "%" << std::endl;
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

bool B4Mesh::IsSpaceInMempool (){

  if (pending_transactions.size() < SIZE_MEMPOOL){
    return true;
  }
  else {
    return false;
  }
}

void B4Mesh::DumpMempool (void)
{
    DEBUG << "Dump MemPool " << std::endl;
    for(auto it = pending_transactions.begin(); it != pending_transactions.end(); ++it)
    {
      DEBUG << it->first << std::endl; 
    }
}

// ****** BLOCK RELATED METHODS *********************
void B4Mesh::BlockTreatment(Block b){

   auto t1 = std::chrono::high_resolution_clock::now();
         
 // DEBUG << " BlockTreatment: Block made by leader: " << b.GetLeader() << " with hash " << b.GetHash().data() << std::endl;
  // Checking if the block is already in the blockgraph
  if (!blockgraph.HasBlock(b.GetHash())){
  //  DEBUG << " BlockTreatment: This block "<< b.GetHash().data() << " is not in the blockgraph " << std::endl;
    // Check if the block is a missing block
    if (IsBlockInMissingList(b.GetHash())){
      // If block is in missing list we erase it hash from the list
    //  DEBUG << " BlockTreatment: This block is a missing block" << std::endl;
      EraseMissingBlock(b.GetHash());
    }
    // Check if the block is already in the waiting list
    if (!IsBlockInWaitingList(b.GetHash())){
      // Check if the block is a merge block.
      if (b.IsMergeBlock()){
      //  DEBUG << " BlockTreatment: Block "<< b.GetHash().data() << " is a merge block" << std::endl;
      //  DEBUG << "Starting synchronization process..." << std::endl;
        //SyncNode(b.GetTransactions());
        StartSyncProcedure(b.GetTransactions());
        AddBlockToBlockgraph(b);
      } else {
        // The block is not a merge block
        // Checking that the parents of this block are in the BG
        std::string ip = node_->consensus_.GetIPFromId(b.GetLeader());
        std::vector<std::string> unknown_parents = GetParentsNotInBG(b.GetParents());
        if (unknown_parents.size() > 0){
          // One or more parents of this block not in the BG
        //  DEBUG << " BlockTreatment: Some of the parents of this block are not in the local BG" << std::endl;
          // Checking that the parents of this block are in the Waiting List:
          unknown_parents = GetParentsNotInWL(unknown_parents);
          if (unknown_parents.size() > 0){
          //  DEBUG << " BlockTreatment: Some of the parents of this block are not in the Waiting List" << std::endl;
            // adding unknown parents to the list of missing blocks
            UpdateMissingList(unknown_parents, ip);
          }
          // Adding the block to the waiting list since parents aren't in BG yet
        //  DEBUG << " BlockTreatment: Adding new block: " << b.GetHash().data() << " to waiting_list: " << std::endl;
          waiting_list[b.GetHash()] = b;
          // Trace purpose
          waiting_list_time[b.GetHash()] = getSeconds();
        } else {
          // All ancestors are known by the local node
          // Adding block to blockgraph
        //  DEBUG << "BlockTreatment: All parents are present in local BG" << std::endl;
          AddBlockToBlockgraph(b);
          // Update the waiting list.
          if (waiting_list.size() > 0){
            UpdateWaitingList();
          }
        }
      }
    } else {
      // The block is already present in waiting list
    //  DEBUG << " BlockTreatment: Block " << b.GetHash() << " already present in waiting list\n  Dumping block..." << std::endl;
      numDumpingBlock++;
    }
  } else {
    // The block is already present in the local blockgraph
  //  DEBUG << " BlockTreatment: Block " << b.GetHash().data() << " already present in blockgraph\n  Dumping block..." << std::endl;
    numDumpingBlock++;
  }

  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> time_treatment = t2 - t1;
  block_treatment_time.push_back(time_treatment.count());
  total_treatment_block_t += time_treatment;
}

void B4Mesh::StartSyncProcedure(std::vector<Transaction> transactions){

  std::string::size_type n;
  std::vector<std::string> myChildless = blockgraph.GetChildlessBlockList();
  std::vector<string>::iterator it;

  DEBUG << "Start Synchronization procedure " << std::endl;

  for (auto &tx : transactions){
    string tmp = tx.GetPayload();
    n = tmp.find(" ");
    int idnode = stoi(tmp.substr(0, n));
    std::string hash = tmp.substr(n+1);

    it = find(myChildless.begin(), myChildless.end(), hash);
    if (it == myChildless.end()){
      RegisterChildless(hash, node_->consensus_.GetIPFromId(idnode));
    }
    ChildlessBlockTreatment(hash, node_->consensus_.GetIPFromId(idnode));
  }
  SendBranchRequest();
  
}

/*
void B4Mesh::SyncNode(vector<Transaction> transactions){
  std::string::size_type n;
  std::vector<std::string> myChildless = blockgraph.GetChildlessBlockList();
  std::vector<string>::iterator it;
  std::vector<string>::iterator it2;

  for (auto &tx : transactions){
    string tmp = tx.GetPayload();
    n = tmp.find(" ");
    int idnode = stoi(tmp.substr(0, n));
    std::string hash = tmp.substr(n+1);

    it = find(myChildless.begin(), myChildless.end(), hash);
    if (it == myChildless.end()){
      if (recover_branch.count(idnode) > 0){
        auto range = recover_branch.equal_range(idnode);
        bool inMap = false;
        for (auto i = range.first; i != range.second; ++i){
          if (i->second == hash){
            inMap = true;
          }
        }
        if (inMap == true){
          continue;
        } else {
          recover_branch.insert(pair<int, string> (idnode, hash));
        }
      } else {
        recover_branch.insert(pair<int, string> (idnode, hash));
      }
      it2 = find(missing_childless.begin(), missing_childless.end(), hash);
      if (it2 == missing_childless.end()){
        missing_childless.push_back(hash);
      }
    }
  }

  for (auto &cb : missing_childless){
    std::vector<string> missing_b = std::vector<string> ();
    int idnode = -1;
    for (auto it = recover_branch.begin(); it != recover_branch.end(); ++it){
      if (it->second == cb){
        missing_b.push_back(cb);
        idnode = it->first;
        goto cnt;
      }
    }
    cnt:;
    DEBUG << " SyncNode: Adding childless block: " << cb << " to list of missing blocks with node: " << idnode << std::endl;
    UpdateMissingList(missing_b, node_->consensus_.GetIPFromId(idnode));
  }

  SendBranchRequest();
}
*/

void B4Mesh::SendBranchRequest(){

  group_branch_hdr_t branch_req;
  branch_req.msg_type = GROUPBRANCH_REQ;
  std::string serie_branch_b_h = "";

  if (node_->consensus_.AmILeader() == true){
    // The leader node ask for missing branches to all new nodes
    std::string serie((char*)&branch_req, sizeof(group_branch_hdr_t));
    for (auto &node : recover_branch){
      serie_branch_b_h = node.second;
      serie += serie_branch_b_h;
      DEBUG << " Leader node asking for this block chain: " << serie_branch_b_h << " to node " << node.first << std::endl;
      ApplicationPacket pkt(ApplicationPacket::CHANGE_TOPO, serie);
      std::string IP = node_->consensus_.GetIPFromId(node.first);
      node_->SendPacket(IP, pkt, false);
    }
  } else {
    for (auto &cb : missing_childless){
      for (auto it = recover_branch.begin(); it != recover_branch.end(); ++it){
        if (it->second == cb){
          serie_branch_b_h = it->second;
          std::string serie((char*)&branch_req, sizeof(group_branch_hdr_t));
          serie += serie_branch_b_h;
          DEBUG << " Follower node asking for this block chain: " << serie_branch_b_h << " to node " << it->first << std::endl;
          ApplicationPacket pkt(ApplicationPacket::CHANGE_TOPO, serie);
          std::string IP = node_->consensus_.GetIPFromId(recover_branch.begin()->first);
          node_->SendPacket(IP, pkt, false);
          goto cntt;
        }
      }
      cntt:;
    }
  }

}

std::vector<std::string> B4Mesh::GetParentsNotInWL(std::vector<std::string> parents){

  std::vector<std::string> ret = std::vector<std::string>();

  for (auto &p : parents){
    if(waiting_list.count(p) > 0){
      DEBUG << "GetParentsNotInWL: Parent : " << p << " is already in waiting_list" << std::endl;
    }
    else {
      DEBUG << "GetParentsNotInWL: Parent : " << p << " is not in Waiting List " << std::endl;
      ret.push_back(p);
    }
  }
  return ret;
}

std::vector<std::string> B4Mesh::GetParentsNotInBG(std::vector<std::string> parents){

  std::vector<std::string> ret = std::vector<std::string>();
  std::map<std::string, Block> bg = blockgraph.GetBlocks();

  for (auto &p : parents){
    if(bg.count(p) > 0){
      DEBUG << "GetParentsNotInBG: Parent: " << p << " already in BG" << std::endl;
    }
    else {
      DEBUG << " GetParentsNotInBG Parent: " << p << " is NOT in Blockgraph" << std::endl;
      ret.push_back(p);
    }
  }
  cout << "Returning ret: " << ret.size() << std::endl;
  return ret;
}

bool B4Mesh::IsBlockInWaitingList(string b_hash){

  for (auto &b : waiting_list){
    if (b_hash == b.first){
      return true;
    }
  }
  return false;
}

bool B4Mesh::IsBlockInMissingList(string b_hash){

  for (auto &pair_m_b : missing_block_list){
    if (b_hash == pair_m_b.first){
      return true;
    }
  }
  return false;
}

void B4Mesh::EraseMissingBlock(string b_hash){
  // Checking that the new block is not a missing blocks
  for (auto pair_m_b = missing_block_list.begin(); pair_m_b != missing_block_list.end(); pair_m_b++){
    if(pair_m_b->first == b_hash){

      //trace purpose
      total_missingBlockTime += getSeconds() - missing_list_time[pair_m_b->first];
      count_missingBlock ++;
      missing_list_time.erase(pair_m_b->first);
      DEBUG << " remove from missing list " << pair_m_b->first << " at " << getSeconds() << " cumul " << total_missingBlockTime << " count "<< count_missingBlock << std::endl;
      DEBUG << " EraseMissingBlock: Erasing block " << pair_m_b->first << " from missing_block_list"<< std::endl;
      missing_block_list.erase(pair_m_b);
      return;
    }
  }
  DEBUG << "EraseMissingBlock: Not a missing block" << std::endl;
}

void B4Mesh::UpdateMissingList(vector<string> unknown_p, std::string ip){
  // Updating missing_block_list
  bool flag = false;

  for (auto &missing_p : unknown_p){
    flag = false;
    for (auto &pair_m_b : missing_block_list){
      if (missing_p == pair_m_b.first){
        DEBUG << "updateMissingList: Parent " << missing_p << " already in list" << std::endl;
        flag  = true;
      }
    }
    if (flag == false){
      DEBUG << "updateMissingList: Parent " << missing_p << " not founded in list" << std::endl;
      DEBUG << "updateMissingList: Adding parent with hash: " << missing_p << " to the missing block list "<< std::endl;
      missing_block_list.push_back(make_pair(missing_p, ip));

	  // Trace purpose
	  DEBUG << "add missing list " << missing_p << " at " << getSeconds() << std::endl;
	  missing_list_time[missing_p] = getSeconds();
    }
  }
}

void B4Mesh::UpdateWaitingList(){
	bool addblock = true; 
	while (addblock){ 
		addblock = false;
		for (auto it = waiting_list.cbegin(); it != waiting_list.cend();){
			DEBUG << "Update waiting list: starting with block: " << it->first.data() << std::endl;
			size_t i = 0;
			for(auto &hash : it->second.GetParents() ){
				if (blockgraph.HasBlock(hash)){
					i++;
				}
			}
			if (it->second.GetParents().size() == i){ // if all parents are in blockgraph
				//Trace purpose
				total_waitingBlockTime += getSeconds() - waiting_list_time[it->first];
        count_waitingBlock ++;
        waiting_list_time.erase(it->first);
				DEBUG << "Update waiting list: All parents are present -> Adding block."<< std::endl;
				AddBlockToBlockgraph(it->second);

				waiting_list.erase(it->first);
				addblock = true;
				break;
			} else {
				DEBUG << "Not all parents from this block are present. keeping the block in the list" << std::endl;
           ++it;
			}
		}
	}
}

void B4Mesh::AddBlockToBlockgraph(Block b){

  std::vector<std::string>::iterator it;
 // bool isMissing = false;

  it = find(missing_childless.begin(), missing_childless.end(), b.GetHash());
  if (it != missing_childless.end())
  {
    missing_childless.erase(it);
  //  isMissing = true;
  }

  // remove the entries when a full branch is recovered
  for (auto it = recover_branch.begin(); it != recover_branch.end();){
    if (it->second == b.GetHash()){
      it = recover_branch.erase(it);
    }
    else {
      ++it;
    }
  }

  // For traces purposes
  /*
  if (missing_childless.size() > 0 && isMissing == true){
    //endmergetime = getSeconds();
  }
  */

  UpdatingMempool(b.GetTransactions());
  DEBUG << " AddBlockToBlockgraph: Adding the block "<< b.GetHash().data() << " to the blockgraph" << std::endl;
  blockgraph.AddBlock(b);
  // For traces propuses
  CreateGraph(b);
  // For visualisation
  AddBlockToVisuFile(b);
}

void B4Mesh::UpdatingMempool (vector<Transaction> transactions)
{
    //DumpMempool();
  for (auto &t : transactions){
    if (pending_transactions.count(t.GetHash()) > 0){
      DEBUG << " UpdatingMempool: Transaction " << t.GetHash() << " founded" << std::endl;
      DEBUG << " UpdatingMempool: Erasing transaction from Mempool... " << std::endl;
      pending_transactions.erase(t.GetHash());

      //Trace purpose
      total_pendingTxTime += getSeconds() - pending_transactions_time[t.GetHash()];
      count_pendingTx ++;
      pending_transactions_time.erase(t.GetHash());
    }
    else {
      DEBUG << " UpdatingMempool:  I don't know this transaction " << t.GetHash() << std::endl;
    }
  }
}

//***************REQUEST_BLOCK METHODS****************************
void B4Mesh::Ask4MissingBlocks()
{
  if (missing_block_list.size() > 0)
  {
    DEBUG << " Ask4MissingBlocks: List of missing parents" << std::endl;
    for (auto &pair_m_b : missing_block_list){
      if (!IsBlockInWaitingList(pair_m_b.first))
      {
        DEBUG << "Ask4MissingBlocks: asking for block: " << pair_m_b.first <<" to node: " << node_->consensus_.GetIdFromIP(pair_m_b.second) << std::endl;
        string serie = pair_m_b.first;
        ApplicationPacket packet(ApplicationPacket::REQUEST_BLOCK, serie);
        if ( pair_m_b.second == "-1")
        {
          node_->BroadcastPacket(packet, false);
        } else {
          node_->SendPacket(pair_m_b.second, packet, false);
        }
      }
    }
  } else {
    DEBUG << " Ask4MissingBlocks: No blocks in waiting list" << std::endl;
  }
}

void B4Mesh::SendBlockto(string hash_p, std::string destAddr){

  if (blockgraph.HasBlock(hash_p)){
    DEBUG << " SendBlockto: Block " << hash_p << " founded!" << std::endl;
    Block block = blockgraph.GetBlock(hash_p);

	  DEBUG << " SendBlockto: Sending block to node: " << destAddr << std::endl;
  	ApplicationPacket packet(ApplicationPacket::BLOCK, block.Serialize());
  	node_->SendPacket(destAddr, packet, true);
  }
    // Case when block is not present...
    // is very unlikely since we already check that this node has this block.
  else {
    DEBUG << " SendBlockto: Block " << hash_p << " not founded!" << std::endl;
    return;
  }
}

//*************CHANGE_TOPO METHODS***********************************
void B4Mesh::StartMerge(){
  // Only leader execute this function

  recover_branch.clear();

  if (node_->consensus_.AmILeader() == false)
    return;

  DEBUG << " Starting Merge at Node: " << std::endl;
  DEBUG << " StartMerge: Leader node: " << node_->consensus_.GetId()
        << " starting the leader synchronization process..." << std::endl;

  Ask4ChildlessBlocks();
}

void B4Mesh::Ask4ChildlessBlocks(){
  // Only leader execute this function

  DEBUG << YELLOW << " Ask4ChildlessBlocks: In leader synch process "<< std::endl;
  childlessblock_req_hdr_t ch_req;
  ch_req.msg_type = CHILDLESSBLOCK_REQ;

  ApplicationPacket packet(ApplicationPacket::CHANGE_TOPO,
      std::string((char*)&ch_req, sizeof(ch_req)));

  // Asking to new nodes for childless blocks
  for (auto &dest : node_->GetnewNodes()){
    DEBUG << YELLOW << " Ask4ChildlessBlocks: Sending CHILDLESSBLOCK_REQ to node: " << node_->consensus_.GetIdFromIP(dest.second) << std::endl;
	  node_->SendPacket(dest.second, packet, false);
  }
  /* new code */
  timer_childless.expires_from_now(std::chrono::seconds(5));
  timer_childless.async_wait(boost::bind(&B4Mesh::timer_childless_fct, this, boost::asio::placeholders::error));
}

void B4Mesh::timer_childless_fct(const boost::system::error_code& /*e*/) 
{
    if (!createBlock)
    {
      mergeBlock = true;
      DEBUG << CYAN << "Timer_childless has expired. Creating merge block" << std::endl;

      GenerateBlocks();

      createBlock = true;
    	node_->ClearnewNodes();
    }
}

void B4Mesh::SendChildlessBlocks(std::string destAddr){

  std::vector<string> myChildless = blockgraph.GetChildlessBlockList();
  std::string serie_hashes = "";

  for (auto &cb : myChildless){
    string tmp_str = cb;
    DEBUG << " SendChildlessBlocks: Childless block founded : " << tmp_str << std::endl;
    serie_hashes += tmp_str;
  }

  if (serie_hashes.size() > 0 ){
    childlessblock_rep_hdr_t ch_rep;
    ch_rep.msg_type = CHILDLESSBLOCK_REP;
    string serie((char*)&ch_rep, sizeof(childlessblock_rep_hdr_t));
    serie += serie_hashes;
    ApplicationPacket pkt(ApplicationPacket::CHANGE_TOPO, serie);
    node_->SendPacket(destAddr, pkt, false);
  } else {
    DEBUG << " SendChildlessBlocks: No childless block founded!"<< std::endl;
  }
}

void B4Mesh::ProcessChildlessResponse(const std::string& msg_payload, std::string senderAddr){
  // Only executed by the leader
  std::string deserie = msg_payload;
  std::vector<std::string> sender_childless = std::vector<std::string> ();
  std::string tmp_hash;

  deserie = deserie.substr(sizeof(childlessblock_rep_hdr_t), deserie.size());

  //deserialisation of the msg_payload
  while ( deserie.size() > 0){
    tmp_hash = deserie.substr(0, 32);
    sender_childless.push_back(tmp_hash);
    deserie = deserie.substr(32, deserie.size());
  }

  for (auto &cb : sender_childless){
    RegisterChildless(cb, senderAddr);
    ChildlessBlockTreatment(cb, senderAddr);
  }

  // List of responses gqthered by the leader node so far
  DEBUG << "Current leader node list of responses" << endl;
  for (auto &node : recover_branch){
    DEBUG << "ProcessChildlessResponse: Node: " << node.first << " has childless block: " << node.second << std::endl;
  } 
  CheckMergeBlockCreation();

}

void B4Mesh::RegisterChildless(std::string childless_hash, std::string senderAddr){
  // Register childless block

  std::int32_t idSender = node_->consensus_.GetIdFromIP(senderAddr);

  DEBUG << "RegisterChildless: Register childless block: " << childless_hash << " from node: " << senderAddr << std::endl;
  // The sender node has already at least one childless block registered
  if (recover_branch.count(idSender) > 0){
    //Getting the hashes matching the sender node key
    auto range = recover_branch.equal_range(idSender);
    bool findHash = false;
    for (auto i = range.first; i != range.second; ++i){
      if (i->second == childless_hash){
        // This chidless block has already been registered
        findHash = true;
      }
    }
    if (findHash == true){
      DEBUG << " Childless hash: " << childless_hash << " has already been registered" << std::endl;
      return;
    } else {
      DEBUG << " Childless hash: " << childless_hash << " is now registered" << std::endl;
      recover_branch.insert(pair<int, string> (idSender, childless_hash));
    }
  } else {
    DEBUG << " Childless hash: " << childless_hash << " is now registered" << std::endl;
    recover_branch.insert(pair<int, string> (idSender, childless_hash));
  }

}

void B4Mesh::ChildlessBlockTreatment(std::string childless_hash, std::string senderAddr){

  std::vector<std::string> myChildless = blockgraph.GetChildlessBlockList();
  std::vector<std::string>::iterator it;

  DEBUG << "ChildlessBlockTreatment" << std::endl;
  
  it = find(missing_childless.begin(), missing_childless.end(), childless_hash); 
  if (it != missing_childless.end()){
    //If this childless block is already registered, then exit
    DEBUG << " ChildlessBlockTreatment: Childless block alredy registered" << std::endl;
    return;
  } else {
    if (find(myChildless.begin(), myChildless.end(), childless_hash) != myChildless.end()){
      DEBUG << "This Childless block: " << childless_hash << " is also a childless block of mine" << std::endl;
      return;
    } else {
      // If childless block is not a childless block of mine
      DEBUG << "This Childless block: " << childless_hash << " is not a childless block of mine" << std::endl;
      if (blockgraph.HasBlock(childless_hash)){
        DEBUG << " ChildlessBlockTreatment: Childless block is present in local blockgraph...Ignoring childless block" << std::endl;
        return;
      } else {
        DEBUG << " ChildlessBlockTreatment: Childless block is not present in blockgraph...Checking if CB is present in waiting list" << std::endl;
        if (IsBlockInWaitingList(childless_hash)){
          DEBUG << " ChildlessBlockTreatment: Childless block is already present in the waiting_list...Ignoring childless block" << std::endl;
          return;
        } else {
          DEBUG << " ChildlessBlockTreatment: Childless block is not present in waiting list...Checking if CB is present in missing_block_list" << std::endl;
          if (IsBlockInMissingList(childless_hash)){
            DEBUG << " ChildlessBlockTreatment: Childless block already in missing_block_list. Ignoring childless block" << std::endl;
            return;
          } else {
            DEBUG << " ChildlessBlockTreatment: Block not in missing_block_list. Adding block to missig_block_list " << std::endl;
            DEBUG << " ChildlessBlockTreatment: Asking for this block " << childless_hash << " branch "  << std::endl;
            // Adding childless block hash in the list of missing block since we now know of it existance
            missing_block_list.push_back(make_pair(childless_hash, senderAddr));
            // adding missing childless block to the list of childless block to recover
            missing_childless.push_back(childless_hash);
            // Trace purpose
            DEBUG << "add missing list " << childless_hash << " at " << getSeconds() << std::endl;
            missing_list_time[childless_hash] = getSeconds();
          }
        }
      }
    }
  }

}

void B4Mesh::CheckMergeBlockCreation(){
  int ans = 0;
  for (auto &n_node : node_->GetnewNodes()){
    auto search = recover_branch.find(n_node.first);
    if (search == recover_branch.end()){
      DEBUG << "Answer of node: " << n_node.first << " not received yet" << std::endl;
      ans++;
    }
  }
  if (ans == 0 && !createBlock){
    // Create Merge BLOCK
    DEBUG << "A merge block can be created now." << std::endl;
    mergeBlock = true;
    createBlock = true;

    GenerateBlocks();
  
  	node_->ClearnewNodes();
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
      DEBUG << " SendBranch4Sync: Sending block hash " << b_grp.GetHash() << " to node " << destAddr << std::endl;
      ApplicationPacket packet(ApplicationPacket::BLOCK, b_grp.Serialize());
      node_->SendPacket(destAddr, packet, true);
    }
  }
}

// ************** GENERATION DES BLOCKS *****************************
bool B4Mesh::TestPendingTxs(){
    // Only leader execute this function

	if (getSeconds() - lastBlock_creation_time > SAFETIME){
		if (pending_transactions.size() > MIN_SIZE_BLOCK){
			DEBUG << YELLOW << " TestPendingTxs: Enough txs to create a block." << std::endl;
			return true;
		}
		else {
			DEBUG << YELLOW << " TestPendingTxs: Not enough transactions to form a block" << std::endl;
			return false;
		}
	}
	DEBUG << YELLOW << " TestPendingTxs: Not allow to create a block so fast"<< std::endl;
    return false;
}

//Select the transactions with a small timestamp.
vector<Transaction> B4Mesh::SelectTransactions(){

  vector<Transaction> transactions;

  while(transactions.size() < SIZE_BLOCK- 1){
    pair<string, double> min_ts = make_pair("0", 9999999);
    for(auto &t : pending_transactions){
      if(find(transactions.begin(), transactions.end(), t.second) != transactions.end()){
        continue;
      }
      else {
        if (t.second.GetTimestamp() < min_ts.second){
          min_ts = make_pair(t.first, t.second.GetTimestamp());
        } else {
          continue;
        }
      }
    }
    if (min_ts.first != "0"){
       DEBUG << "Getting tx: " << min_ts.first << " with Timestamp: " << min_ts.second << std::endl;
       transactions.push_back(pending_transactions[min_ts.first]);
    } else {
      break;
    }  
  }
  return transactions;
}


void B4Mesh::GenerateBlocks(){
   auto t1 = std::chrono::high_resolution_clock::now();
  // Only leader execute this function
  
  std::vector<string> myChildless = blockgraph.GetChildlessBlockList();
  std::vector<std::string> p_block = vector <string> ();
  
	// Initialize the block
	Block block;
	block.SetLeader(node_->consensus_.GetId());
	block.SetGroupId(node_->GetGroupId());
 // DEBUG << " GenerateBlock: Block's groupId is: " << node_->GetGroupId() << std::endl;
	// Getting Parents of the futur Block
	for (auto& parent : myChildless){
		p_block.push_back(parent);
	}
	// If the block to create is a merge block
	if (mergeBlock || p_block.size() > 1 ){
		std::vector<Transaction> transactions = std::vector<Transaction> ();
   // DEBUG << " GenerateBlock: Creation of a merge block" << std::endl;
	//	DEBUG << " GenerateBlock: Merge block transactions are: " << std::endl;
		for (auto hash : p_block){
      Transaction t;
      string payload = to_string(node_->consensus_.GetId()) + " " + hash;
   //   DEBUG << payload << endl;
      t.SetPayload(payload);
      t.SetTimestamp(getSeconds());
      transactions.push_back(t);
		}

		for (auto &node : recover_branch){
			Transaction t;
			std::string payload = to_string(node.first) + " " + node.second;
	//		DEBUG << payload << std::endl;
			t.SetPayload(payload);
			t.SetTimestamp(getSeconds());
			transactions.push_back(t);
		}

		block.SetTransactions(transactions);

    // Adding the new parents
		if(missing_childless.size() > 0){
			for (auto &pb : missing_childless){
				if(find(p_block.begin(), p_block.end(), pb) == p_block.end()){
					p_block.push_back(pb);
				}
			}
		}
		mergeBlock = false;
	}
	else {
		std::vector<Transaction> transactions = SelectTransactions();
		block.SetTransactions(transactions);
		// Remove transactions from the list of pending transactions
		/* Transactions from the leader need to be removed Here
			* and not at the block reception because the leader's Mempool size
			* is the critirial to create blocks. If transactions are not removed
			* blocks are going to be created with repeated txs
			*/
			//for (auto &t : transactions)
			//  pending_transactions.erase(t.GetHash()); //Not in case of POC
	}

 // DEBUG << " GenerateBlock: Parents of new block are: " << std::endl;
  /*
  for (auto &p : p_block){
  //  DEBUG << " GenerateBlock: Block: " << p << std::endl;
  }
  */

	block.SetParents(p_block);

	int index = -1;
	for (auto &p : p_block){
		index = max(index, blockgraph.GetBlock(p).GetIndex());
	}
	block.SetIndex(index+1);
	block.SetTimestamp(getSeconds());

	lastBlock_creation_time = getSeconds();

//	DEBUG << " Block size is:   " << block.GetSize() <<  std::endl;
	SendBlockToConsensus(block);
 
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> time_treatment = t2 - t1;
  block_creation_time.push_back(time_treatment.count());
  total_treatment_block_t += time_treatment;

  BlockTreatment(block);
}

void B4Mesh::SendBlockToConsensus(Block b)
{
  // If a consensus protocol is used to replicate blocks in all nodes
  // Here the block shall be delegated to the consensus protocol.
  // Since consensusd protocol only elects a leader, we only send the block to all nodes.
	DEBUG << GREEN << " SendBlockToConsensus:   " << b.GetHash() << RESET << std::endl;
	DEBUG << "sending b "<< b << std::endl;
	string serie = b.Serialize();
	ApplicationPacket packet(ApplicationPacket::BLOCK, serie);
	node_->BroadcastPacket(packet, true);
}


// *************** TRACES AND PERFORMANCES ***********************************

void B4Mesh::CreateGraph(Block &b){
  vector<string> parents_of_block = b.GetParents();
  pair<int,pair<int,int>> create_blockgraph = pair<int,pair<int,int>> ();

  for (auto &p : parents_of_block){
    if (p == "01111111111111111111111111111111"){
      create_blockgraph = make_pair(0,
                                  make_pair(0,stoi(b.GetHash())));
      blockgraph_file.push_back(create_blockgraph);
    } else {
      create_blockgraph = make_pair(stoi(b.GetGroupId()),
                                  make_pair(stoi(p),stoi(b.GetHash())));
      blockgraph_file.push_back(create_blockgraph);
    }
  }
}

int B4Mesh::SizeMempoolBytes(void){
    int ret = 0;
    for (auto &t : pending_transactions){
    ret += t.second.GetSize();
    }
    return ret;
}

void B4Mesh::AddBlockToVisuFile(Block b){

    json j;
    j["node"]["hash"] =  stoi(b.GetHash());
    j["node"]["groupId"] = stoi(b.GetGroupId());

    std::vector<std::string> parents_of_block = b.GetParents();
    std::vector<int> parents_of_block_prettify;

    for (auto &p : parents_of_block){
      if (p == "01111111111111111111111111111111"){
          parents_of_block_prettify.push_back(0);
      } else {
          parents_of_block_prettify.push_back(stoi(p));
      }
    }

    j["node"]["parent"] = parents_of_block_prettify;

    visuBlock << j << std::endl;

}

void B4Mesh::GenerateResults()
{
  std::cout << " GenerateResults " << std::endl;

	std::cout << "\nB4mesh: Stop B4Mesh on node : " << node_->consensus_.GetId() << std::endl;
  std::cout << "************** BLOCKGRAPH RELATED PERFORMANCES **************" << std::endl;
	std::cout << "B4mesh: Execution time : " << getSeconds() << "s" << std::endl;
  std::cout << "B4Mesh: Size of the blockgraph (bytes): " << blockgraph.GetByteSize() << std::endl;
  std::cout << "B4Mesh: Packets lost due to messaging pb: " << lostPacket << std::endl;
  std::cout << "************** BLOCK RELATED PERFORMANCES **************" << std::endl;
	std::cout << "B4mesh: Total number of blocks : " << blockgraph.GetBlocksCount() << std::endl;
	std::cout << "B4Mesh: Mean size of a block in the blockgraph (bytes): " << blockgraph.GetByteSize()/blockgraph.GetBlocksCount() << std::endl;
  std::cout << "B4Mesh: Number of dumped blocks (already in waiting list or in the blockgraph): " <<  numDumpingBlock << std::endl;
  std::cout << "B4Mesh: Number of remaining blocks' references in the missing_block_list: " << missing_block_list.size() << std::endl;
  for (auto &b : missing_block_list){
		std::cout << "B4Mesh: - Block #: " << b.first.data() << " is missing " << std::endl;
	}
	std::cout << "B4Mesh: Mean time that a blocks' reference spends in the missing_block_list: " << total_missingBlockTime / count_missingBlock << std::endl;
  std::cout << "B4Mesh: Number of remaining blocks in the waiting_list: " << waiting_list.size() << std::endl;
    for (auto &b : waiting_list){
      std::cout << "B4Mesh: - Block #: " << b.first << " is in waiting_list " << std::endl;
    }
	std::cout << "B4Mesh: Mean time that a block spends in the waiting_list: " << total_waitingBlockTime / count_waitingBlock << std::endl;
  std::cout << "************** TRANSACTIONS RELATED PERFORMANCES **************" << std::endl;
	std::cout << "B4mesh: Total number of transactions in the blockgraph: " << blockgraph.GetTxsCount() << std::endl;
  std::cout << "B4Mesh: Number of transactions generated by this node: " << numTxsG << std::endl;
	std::cout << "B4mesh: Number of committed transactions per seconds: " << (float)(blockgraph.GetTxsCount())/getSeconds() << std::endl;
  std::cout << "B4Mesh: Size of all transactions in the blockgraph (bytes): " << blockgraph.GetTxsByteSize() << std::endl;
	std::cout << "B4Mesh: Mean number of transactions per block: " << blockgraph.MeanTxPerBlock() << std::endl;
	std::cout << "B4Mesh: Mean time that a transaction spend in the mempool: " << total_pendingTxTime / count_pendingTx << std::endl;
	std::cout << "B4Mesh: Number of remaining transactions in the mempool: " << pending_transactions.size() << std::endl;
	std::cout << "B4Mesh: Number of re transaction: " << numRTxsG << std::endl;
  std::cout << "B4Mesh: Transactions lost due to mempool space: " << lostTrans << std::endl;
  std::cout << "B4Mesh: Number of droped transactions (already in blockgraph or in mempool):  " << numDumpingTxs << std::endl;
	std::cout << "B4Mesh: Transactions with multiple occurance in the blockgraph: "  << std::endl;
	int TxRep = blockgraph.ComputeTransactionRepetition();
	std::cout << TxRep << std::endl;


  // --- Performances of the blockgraph ---
  ofstream output_file;
  char filename[50];
  sprintf(filename, "performances-%d.txt", node_->consensus_.GetId());
  output_file.open(filename, ios::out);
  output_file << " Execution time : " << getSeconds() << "s" << std::endl;
  output_file << " Mean block creation time : " << total_process_block_t.count() / (blockgraph.GetBlocksCount()-1) << "ms" << std::endl;
  output_file << " Mean block treatment time : " << total_treatment_block_t.count() / (blockgraph.GetBlocksCount()-1) << "ms" << std::endl;
  output_file << " Mean tx treatment time : " << total_treatment_tx_t.count() / blockgraph.GetTxsCount() << "ms" << std::endl;
  output_file << " Size of the blockgraph : " << blockgraph.GetByteSize() / 1000 << "Kb" << std::endl;
  output_file << " Packets lost due to messaging pb: " << lostPacket << std::endl;
  output_file << "************** BLOCK RELATED PERFORMANCES **************" << std::endl;
  output_file << "B4mesh: Total number of blocks : " << blockgraph.GetBlocksCount() << std::endl;
  output_file << "B4Mesh: Mean size of a block in the blockgraph : " << (blockgraph.GetByteSize()/blockgraph.GetBlocksCount()) / 1000 << "Kb" << std::endl;
  output_file << "B4Mesh: Number of dumped blocks (already in waiting list or in the blockgraph): " <<  numDumpingBlock << std::endl;
  output_file << "B4Mesh: Number of remaining blocks' references in the missing_block_list: " << missing_block_list.size() << std::endl;
  for (auto &b : missing_block_list){
    output_file << "B4Mesh: - Block #: " << b.first.data() << " is missing " << std::endl;
	}
  output_file << "B4Mesh: Mean time that a blocks' reference spends in the missing_block_list: " << total_missingBlockTime / count_missingBlock << "s" << std::endl;
  output_file << "B4Mesh: Number of remaining blocks in the waiting_list: " << waiting_list.size() << std::endl;
    for (auto &b : waiting_list){
      output_file << "B4Mesh: - Block #: " << b.first << " is in waiting_list " << std::endl;
    }
  output_file << "B4Mesh: Mean time that a block spends in the waiting_list: " << total_waitingBlockTime / count_waitingBlock << "s" << std::endl;
  output_file << "************** TRANSACTIONS RELATED PERFORMANCES **************" << std::endl;
  output_file << "B4mesh: Total number of transactions in the blockgraph: " << blockgraph.GetTxsCount() << std::endl;
  output_file << "B4Mesh: Number of transactions generated by this node: " << numTxsG << std::endl;
  output_file << "B4mesh: Number of committed transactions per seconds: " << (float)(blockgraph.GetTxsCount())/getSeconds() << std::endl;
  output_file << "B4Mesh: Size of all transactions in the blockgraph : " << blockgraph.GetTxsByteSize() / 1000 <<  "kB" << std::endl;
  output_file << "B4Mesh: Mean number of transactions per block: " << blockgraph.MeanTxPerBlock() << std::endl;
  output_file << "B4Mesh: Mean time that a transaction spendS in the mempool: " << total_pendingTxTime / count_pendingTx << "s" << std::endl;
  output_file << "B4Mesh: Number of remaining transactions in the mempool: " << pending_transactions.size() << std::endl;
  output_file << "B4Mesh: Number of re transaction: " << numRTxsG << std::endl;
  output_file << "B4Mesh: Transactions lost due to mempool space: " << lostTrans << std::endl;
  output_file << "B4Mesh: Number of droped transactions (already in blockgraph or in mempool):  " << numDumpingTxs << std::endl;
  output_file << "B4Mesh: Transactions with multiple occurance in the blockgraph: "  << TxRep << std::endl;


	// ---The blockgraph into file. ----
	ofstream output_file1;
	char filename1[50];
	sprintf(filename1, "blockgraph-%d.txt", node_->consensus_.GetId());
	output_file1.open(filename1, ios::out);
	output_file1 << "#BlockGroup" << " " << "ParentBlock" << " " << "BlockHash" << std::endl;
	for (auto &it : blockgraph_file){
		output_file1 << it.first << " " << it.second.first << " " << it.second.second << std::endl;
	}
	output_file1.close();

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

  // ---Block creation processing time. ----
	ofstream output_file3;
	char filename3[50];
	sprintf(filename3, "block_creation_time-%d.txt", node_->consensus_.GetId());
	output_file3.open(filename3, ios::out);
	for(auto &it : block_creation_time)
    {
      output_file3 << it << std::endl; 
    }
	output_file3.close();

  // ---Block treatment processing time. ----
	ofstream output_file4;
	char filename4[50];
	sprintf(filename4, "block_treatment_time-%d.txt", node_->consensus_.GetId());
	output_file4.open(filename4, ios::out);
	for(auto &it : block_treatment_time)
    {
      output_file4 << it << std::endl; 
    }
	output_file4.close();

  // ---Transaction treatment processing time. ----
	ofstream output_file5;
	char filename5[50];
	sprintf(filename5, "tx_treatment_time-%d.txt", node_->consensus_.GetId());
	output_file5.open(filename5, ios::out);
	for(auto &it : tx_treatment_time)
    {
      output_file5 << it << std::endl; 
    }
	output_file5.close();

	// close live file
  visuBlock.close();
	visuMemPool.close();
    
}
