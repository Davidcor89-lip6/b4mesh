#include "blockgraph.hpp"

Blockgraph::Blockgraph ()
{
  txsCount = 0;
  txsSize = 0;

  Block genesisBlock("0", 0, 0, "0", vector<string> (), 0.0, vector<Transaction> ());
  AddBlock(genesisBlock);
}

Blockgraph::Blockgraph (const Blockgraph &b){
  blocks = b.blocks;
}

Blockgraph::~Blockgraph (void)
{
}

int Blockgraph::GetByteSize () {
  int size=0;

  for (auto& b : blocks){
    size += b.second.GetSize();
  }
  return size;
}
/*
int Blockgraph::GetTxsCount (){
    int totalTx = 0;

    for (auto& b : blocks){
      totalTx = totalTx + b.second.GetTransactions().size();
    }
    return totalTx;
}
*/

int Blockgraph::GetTxsCount (){
  return txsCount;
}

/*
int Blockgraph::GetTxsByteSize (){
  int totalBytes = 0;

  for (auto b : blocks){
    totalBytes += b.second.CalculeTxsSize();
  }
  return totalBytes;
}
*/

int Blockgraph::GetTxsByteSize (){
  return txsSize;
}

int Blockgraph::GetBlocksCount () const{
  return blocks.size();
}

string Blockgraph::GetGroupId(string hash){
  string ret = "";
  for (auto &b : blocks){
    if (b.second.GetHash() == hash){
      ret = b.second.GetGroupId();
      return ret;
    }
  }
  return ret = "0000";
}

vector<string> Blockgraph::GetAllBlockHashes () const{
  vector<string> ret = vector<string>();
  for (auto &b : blocks){
    ret.push_back(b.second.GetHash());
  }
  return ret;
}

const map<string, Block>& Blockgraph::GetBlocks () const{
  return blocks;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Blockgraph::AddBlock  (Block& newBlock){
    bool present = HasBlock(newBlock);
    if (!present){
      blocks.insert({newBlock.GetHash(), newBlock});
      txsCount += newBlock.GetTxsCount();
      txsSize += newBlock.CalculeTxsSize();
    }
    // If block already present in Blockgraph -> reject the block
}

bool Blockgraph::HasBlock (Block& block) {
  return (blocks.count(block.GetHash()) == 1);
}

bool Blockgraph::HasBlock (string hash) {
  return (blocks.count(hash) == 1);
}

Block Blockgraph::GetBlock (string hash) {
  if (HasBlock(hash))
    return blocks[hash];
  return Block("-1", -1, -1, "-1", vector<string> (), -1.0, vector<Transaction> ());
}

vector<string> Blockgraph::GetChildren (Block &block){

  vector <string> children = vector<string> ();

  for (auto& b : blocks){
    if (block.IsChild(b.second))
      children.push_back(b.first);
  }
  return children;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

vector<Block> Blockgraph::GetBlocksFromGroup (string groupId) {

  vector<Block> blocks_group = vector<Block> ();

  for(auto& b : blocks){
    if (b.second.GetGroupId() == groupId){
      blocks_group.push_back(b.second);
    }
  }
  return blocks_group;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


vector<Block> Blockgraph::GetChildlessBlocks (){

  set<string> childless;
  vector<Block> chless;

  // Initialisation
  for (auto&b : blocks)
    childless.insert(b.first);

  for (auto& b : blocks){
    for (auto& p : b.second.GetParents())
        childless.erase(p);
  }

  for (auto &e : childless){
    chless.push_back(GetBlock(e));
  }

  return chless;
  //return vector<string>(childless.begin(), childless.end());
}


vector<string> Blockgraph::GetChildlessBlockList (){
  vector<string> ret = vector<string> ();
  set<string> childless;

  for (auto& b : blocks)
    childless.insert(b.first);

  for (auto& b : blocks){
    for (auto& p : b.second.GetParents())
        childless.erase(p);
  }

  for (auto &e : childless){
    ret.push_back(e);
  }

return ret;
}


bool Blockgraph::IsChildless(Block &block){

  for (auto& b : blocks){
    for (auto& p : b.second.GetParents()){
      if (p == block.GetHash()){
        return false;
      }
    }
  }
  return true;
}

bool Blockgraph::IsTxInBG(Transaction &t){
  for (auto& b : blocks){
    for (auto& tx : b.second.GetTransactions()){
      if (tx.GetHash() == t.GetHash()){
        return true;
      }
    }
  }
  return false;
}

int Blockgraph::CountRepTxInBlockGraph (Transaction &t){
  int count = 0;
  for (auto& b : blocks){
    for (auto& tx : b.second.GetTransactions()){
      if (tx.GetHash() == t.GetHash()){
        count++;
      }
    }
  }
  return count;
}

int Blockgraph::ComputeTransactionRepetition (){
  std::map<std::string, int> mem;
  int count = 0;
  for (auto& b : blocks){
    for (auto& tx : b.second.GetTransactions()){
      int res = CountRepTxInBlockGraph(tx);
      if (res > 1){
        auto it = mem.find(tx.GetHash());
        if (it == mem.end()){
          std::cout << stoi(tx.GetHash()) << " : " << res << std::endl;
          count++;
          mem[tx.GetHash()] = res;
        }
      }
    }
  }
  return count;
}

float Blockgraph::MeanTxPerBlock(){
  int txsInBlock = 0;
  for (auto& b : blocks){
    txsInBlock += b.second.GetTransactions().size();
  }
  float mean = txsInBlock/GetBlocksCount();
  return mean;
}

ostream& operator<< (std::ostream &out, const Blockgraph &bg){

  out << "Blockgraph (" << bg.GetBlocksCount() << ",";
  out << "[";
  for (auto b : bg.GetBlocks())
    out << b.second << ",";
  out << "])";

  return out;
}

Blockgraph& Blockgraph::operator=(const Blockgraph &bg){
  blocks = bg.blocks;

  return *this;
}
