#ifndef B4MESH_B4MESH_P
#define B4MESH_B4MESH_P

#include "application_packet.hpp"
#include "transaction.hpp"
#include "block.hpp"
#include "blockgraph.hpp"

#include <vector>
#include <utility>
#include <random>
#include <limits>
#include <queue>
#include <math.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/random.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/math/distributions/exponential.hpp>

using boost::asio::ip::tcp;

#include "node.hpp"

#include "configs.hpp"

class B4Mesh
{

    public:
    static const int TIME_UNIT = 10;
    enum{CHILDLESSBLOCK_REQ, CHILDLESSBLOCK_REP, GROUPBRANCH_REQ,
            GROUPCHANGE_REQ};
    enum{RAFT, B4MESH_RAFT};

    public:
    typedef struct childlessblock_req_hdr_t {
        int msg_type;             // type of the message
    } childlessblock_req_hdr_t;

    typedef struct childlessblock_rep_hdr_t {
        int msg_type;             // type of the message
    } childlessblock_rep_hdr_t;
    // Message type for node synchronization. Recovers a branch
    typedef struct group_branch_hdr_t {
        int msg_type;             // type of the message
    } group_branch_hdr_t;

    public:
    /**
     * Constructors and destructor
     */
    B4Mesh(node* node, boost::asio::io_context& io_context, short port, std::string myIP);
    ~B4Mesh();

    public:
    /**
     * Callback function to process incoming packets on the socket passed in
     * parameter. Basically, this function implement de protocol of the
     * blockgraph application.
     */
    void ReceivePacket(std::string packet, std::string ip);

    /* Retunes the type of message serialized in the payload */
    int ExtractMessageType(const std::string& msg_payload);

    /**
     * Function that generate the blockgraph file
     */
    void CreateGraph(Block &block);
    /**
     * This function generates files for performances
     */
    void GenerateResults();

    public:
    /**
     * A recursive method to make the node generate transactions and broadcast
     * it to the local group.
     */
    void GenerateTransactions();

    void SendTransaction(Transaction t);
    /**
     * A method that generates a new block when Txs in Mempool are enough to
     * create a block. -Only Leader -Broadcast block to the local group
     */
    void GenerateBlocks();
    /**
     * Allow to select the transaction to put inside a block
     */
    vector<Transaction> SelectTransactions();

    void SendBlockToConsensus(Block b);
    /*
        * This method checks the Mempool state every second. If Txs in Mempool are
        * sufficients, then invok GenerateBlocks(); - Can also invok ChangeLeader()
        */
    bool TestPendingTxs();
    /**
     * Checks if a transaction is already present in the mempool
     */
    bool IsTxInMempool (Transaction t);
    /**
     * Checks that the mempool of the node is not full
     */
    bool IsSpaceInMempool();
    /**
     * Dump the mempool
     */
    void DumpMempool (void);
    /**
     *  Check if the transaction is already present in the blockgraph
     */
    bool IsTxInBlockGraph (Transaction t);
    /**
     * Updates the Txs Mempool once the Block is added to the BG
     */
    void UpdatingMempool(vector<Transaction> transactions);


    public:
    //  BLOCKGRAPH GROUP MANAGMENT RELATED
    /**
     * Do the tratment of a block befor adding it to the blockgraph
     */
    void BlockTreatment(Block block);
    /**
     * Checks is a block is the waiting list
     */
    bool IsBlockInWaitingList( std::string b_hash);
    /**
     * Check if the parents of the received block are known by the node
     */
    bool AreParentsKnown(Block &block);
    /**
     * Do the tratment of a transaction befor adding it to the mempool
     */
    void TransactionsTreatment(Transaction t);


    void StartMerge();


    /**
     * Check if the parents of the received block are known by the node
     */
    bool CheckBlockParents(Block &block);
    /**
     * Get the hashes of unknown parents of the received block
     */
    std::vector<std::string> GetUnknownParents(vector<string> parents);
    /**
     * This method is invok when a node doesn't know the parent(s) of a block
     * It will ask for them via their BlockHash to the block sender.
     */
    void Ask4ParentBlock(std::string hash, std::string ip);
    /**
     * It's the answer of Ask4ParentBlock(). It sends the missing parent of a block
     * to the receiver. (Usually is the leader executing this method)
     */
    void SendParentBlock(std::string hash, std::string ip);
    //------------------------------
    /**
     * Method executed by the leader when a topology change is detected
     * This method causes a CHILDLESSBLOCK_REQ
     */
    void CheckBlockgraphSync();
    /**
     * Send the childless blocks to leader upon a CHILDLESSBLOCK_REQ
     */
    void SendChildlessBlocks(std::string ip);
    /**
     * The leader checks 4 missing side-chains from followers CHILDLESSBLOCK_REP
     * If a childless block groupId is unkwnow, leader ask for the whole branch
     */
    void CheckBranchesSync(const std::string& msg_payload, std::string ip);
    /**
     * Send a full side-chain to the leader upon a GROUPBRANCH_REQ
     */
    void SendBranch4Sync(const std::string& msg_payload, std::string ip);
    /**
     *  Start the fast synchronization process of any node
     *  after receiving a merge block
     */
    void SyncNode(std::vector<std::string> unknown_p, std::string ip);
    /**
     * Updates the missing_parents_list. If new block is a missing parent
     * it will ERASE it form the list
     */
    void EraseMissingBlock(string b_hash);
    /**
     * Updates the missing_parents_list. If new block has missing parents
     * it will ADD them to the missing_parents_list
     */
    void UpdateMissingList(vector<string> unknown_p, std::string ip);
    /**
     *  Checks that the parents of the blocks in the waiting list are
     *  all present in the local BG. If so, it add the block to the local
     *  BG and erase it from the waiting list
     */
    void UpdateWaitingList();
    /**
     *  Add the block to the local blockgraph
     */
    void AddBlockToBlockgraph(Block block);

    bool IsBlockMerge(vector<string> parents);
    //-----------------------------
    /**
     * This method checks every 5s if there still missing blocks in the
     * missing_parents_list. If so, it will ask for them to the node who
     * send the child block of the missing parent.
     */
    void Ask4MissingBlocks();
    /**
     * Checks if the hash of the block received as paremeter is present
     * in the missing_parents_list
     */
    bool IsBlockInMissingList(std::string b_hash);
    /**
     * Updates the missing_parents_list. If new block is a missing parent
     * it will ERASE it form the list
     */
    void updateMissingList(std::string b_hash);
    /**
     * Updates the missing_parents_list. If new block has missing parents
     * it will ADD them to the missing_parents_list
     */
    void updateMissingList(std::vector<std::string> unknown_p, std::string ip);

    private:
    /**
     * Getting performances
     */
    /**
     * Calcul the size of the mempool in bytes
     * (Only needed for getting performances of the system)
     */
    int SizeMempoolBytes ();

    public:
    /*
        * Getting performances using Traces
        */
    void TxSizeBuffer();

    public:
    /* Traces for B4Mesh App */
    unsigned int sizemempool;
    std::vector <std::pair<int, std::pair <int, int>>> blockgraph_file;

    private:
    unsigned int blocktxsSize; // the critiria of num of txs in mempool to create a block
    int lostTrans; // counts the number of transactions lost because of space limit
    int lostPacket; // counts the number of packet lost because of network jamming
    int numTxsG; // The number of transaction generated by the local node
    int numRTxsG; // The number of retransaction 
    // List of missing blocks in the local BG <BlockHash, IpOfNodeWhoHasTheMissingParent>
    std::vector<std::pair<std::string, std::string>> missing_parents_list;
    std::string groupId;   // GroupId of a group
    std::map<std::string, Transaction> pending_transactions;  // mempool of transactions
    std::map<std::string, Block> waiting_list; // blocks waiting for their ancestors
    Blockgraph blockgraph;  // The database of blocks

private:
    //parent
    node* node_;
    std::string mIP_;
    std::chrono::time_point<std::chrono::steady_clock> time_start;
    double getSeconds() {
        std::chrono::time_point<std::chrono::steady_clock> time_end = std::chrono::steady_clock::now();
        return std::chrono::duration<double>(time_end - time_start).count();
    }
    boost::asio::io_context& io_context_;
    
    // random variable
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> dist;
    boost::variate_generator<boost::mt19937&, boost::exponential_distribution<> > dist_exp;
 
    // generate transaction
    void timer_generateT_fct (const boost::system::error_code& /*e*/) {GenerateTransactions();}
    boost::asio::steady_timer timer_generateT;

    // other recurrent call
    void timer_recurrentTask_fct (const boost::system::error_code& /*e*/);
    boost::asio::steady_timer timer_recurrentTask;

};



#endif //B4MESH_B4MESH_P