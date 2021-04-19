<<<<<<< Updated upstream
=======
/*
  This class implements the definition of a "block" as a data structure holding several transactions.
*/

>>>>>>> Stashed changes
#include <b4mesh/core_engine/block.hpp>

Block::Block(){
  hash = string(HASH_SIZE, 0);
  index = 0;
  leader = 0;
  groupId = string(HASH_SIZE, 0);
  parents = vector<string>();
  timestamp = 0.0;
  transactions = vector<Transaction>();

  size = CalculateSize();
  CalculateHash();
}

Block::Block(string hash, int index, int leader, string groupId,
             vector<string> parents, double timestamp,
             vector<Transaction> transactions){

<<<<<<< Updated upstream
  this->hash =                 hash + string(HASH_SIZE - hash.size(), '1');
  this->index =                index;
  this->leader =               leader;
  this->groupId = groupId + string(HASH_SIZE - groupId.size(), '0');
  this->parents =              parents;
  this->timestamp =            timestamp;
  this->transactions =         transactions;

  size = CalculateSize();

=======
  this->hash =            hash + string(HASH_SIZE - hash.size(), '1');
  this->index =           index;
  this->leader =          leader;
  this->groupId =         groupId + string(HASH_SIZE - groupId.size(), '0');
  this->parents =         parents;
  this->timestamp =       timestamp;
  this->transactions =    transactions;

  size = CalculateSize();
>>>>>>> Stashed changes
}

Block::Block(const Block &b){

  hash =                 b.hash;
  index =                b.index;
  leader =               b.leader;
  groupId =              b.groupId;
  parents =              b.parents;
  timestamp =            b.timestamp;
  transactions =         b.transactions;
  size =                 b.size;
}

<<<<<<< Updated upstream
Block::Block(const string &serie){
  const block_t *p = (block_t*) serie.data();

  hash = string(p->hash, HASH_SIZE);
  index = p->index;
  leader = p->leader;
  groupId = string(p->groupId, HASH_SIZE);
  timestamp = p->timestamp;
  size = p->size;

  int tx_count = p->tx_count;
  int parents_count = p->parents_count;

  const char *p_parents = (const char*) p + sizeof(block_t);
  for(int i=0; i<parents_count*HASH_SIZE; i += HASH_SIZE)
    parents.push_back(string(p_parents+i, HASH_SIZE));

  const char *p_transactions = p_parents + parents_count * HASH_SIZE;
  int transactions_offset = 0;
  int remaining_bytes = serie.size() - (p_transactions - (const char*)p);
  for (int i=0; i<tx_count; ++i){
    Transaction t(string(p_transactions+transactions_offset, remaining_bytes));
    transactions.push_back(t);
    transactions_offset += t.GetSize();
    remaining_bytes -= t.GetSize();
  }

  size = CalculateSize();
}

Block::~Block(){

}

=======
// Block deserialization
Block::Block(const string &serie){
  try 
  {
    const block_t *p = (block_t*) serie.data();
    hash = string(p->hash, HASH_SIZE);
    index = p->index;
    leader = p->leader;
    groupId = string(p->groupId, HASH_SIZE);
    timestamp = p->timestamp;
    size = p->size;
    int tx_count = p->tx_count;
    int parents_count = p->parents_count;
    const char *p_parents = (const char*) p + sizeof(block_t);

    for (int i=0; i<parents_count*HASH_SIZE; i += HASH_SIZE)
    {
      parents.push_back(string(p_parents+i, HASH_SIZE));
    }

    const char *p_transactions = p_parents + parents_count * HASH_SIZE;
    int transactions_offset = 0;
    int remaining_bytes = serie.size() - (p_transactions - (const char*)p);

    for (int i=0; i<tx_count; ++i)
    {
      Transaction t(string(p_transactions+transactions_offset, remaining_bytes));
      transactions.push_back(t);
      transactions_offset += t.GetSize();
      remaining_bytes -= t.GetSize();
    }
    size = CalculateSize();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    std::cout << "Error in the deserialization of the block" << std::endl;
    throw e;
  }
}

Block::~Block(){
}

// Getters, Setters and operator

>>>>>>> Stashed changes
string Block::GetHash() const{
  return hash;
}

void Block::SetHash(string hash){
  this->hash = hash;
}

int Block::GetIndex(){
  return index;
}

void Block::SetIndex(int index){
  this->index = index;
  CalculateHash();
}

int Block::GetLeader(){
  return leader;
}

void Block::SetLeader(int leader){
  this->leader = leader;
  CalculateHash();
}

string Block::GetGroupId(){
  return groupId;
}

void Block::SetGroupId(string groupId){
  this->groupId = groupId + string(HASH_SIZE - groupId.size(), 0);
  CalculateHash();
}

vector<string> Block::GetParents() const{
  return parents;
}

void Block::SetParents(vector<string> parents){
  this->parents = parents;

  for (auto &p : this->parents)
    p = p + string(HASH_SIZE - p.size(), 0);

  size = CalculateSize();
  CalculateHash();
}

double Block::GetTimestamp(){
  return timestamp;
}

void Block::SetTimestamp(double timestamp){
  this->timestamp = timestamp;
  CalculateHash();
}

vector<Transaction> Block::GetTransactions() const{
  return transactions;
}

void Block::SetTransactions(vector<Transaction> transactions){
  this->transactions = transactions;
  size = CalculateSize();
  CalculateHash();
}

int Block::GetSize(){
  return size;
}

void Block::SetSize(int size){
  this->size = size;
  CalculateHash();
}

<<<<<<< Updated upstream
bool Block::IsParent(Block &block){
  for (string p_hash : block.GetParents()){
    if(GetHash() == p_hash)
      return true;
  }
  return false;
}

bool Block::IsChild(Block &block) {
  return block.IsParent(*this);
}

bool Block::IsPartOfGroup(string groupId) {
  if(GetGroupId() == this->groupId)
    return true;
  else
    return false;
}

bool Block::IsMergeBlock(Block &block) {
  if(block.GetParents().size() > 1)
    return true;
  else
    return false;
}

bool Block::IsMergeBlock (vector<string> parents) {

  for (auto &p : parents){
    bool found = false;
    for (auto &my_p : this->parents)
      if (p == my_p){
        found = true;
        break;
      }
    if (!found) return false;
  }
  return true;
}

string Block::CalculateHash(){
  hash = string(HASH_SIZE, 0);
  string serie = Serialize();
  hash = hashing(serie, HASH_SIZE);
  return hash;

}

=======
>>>>>>> Stashed changes
Block& Block::operator= (const Block &b){
  hash =                 b.hash;
  index =                b.index;
  leader =               b.leader;
  groupId =              b.groupId;
  parents =              b.parents;
  timestamp =            b.timestamp;
  transactions =         b.transactions;
  size =                 b.size;

  return *this;
}

bool Block::operator==(const Block &b){
  return hash == b.hash;
}

ostream& operator<<(std::ostream &out, const Block &b){
  out << "Block(" << dump(b.hash.data(), 10) << ",";
  out << b.index << ",";
  out << b.leader << ",";
  out << dump(b.groupId.data(), 10) << ",";
  out << b.timestamp << ",";
  out << b.size << ",";
  out << "[";
  for (auto &h : b.parents)
    out << dump(h.data(), 10) << ",";
  out << "],";
  out << "[";
  if (b.transactions.size() <= 2){
    for (auto &t : b.transactions)
      out << t << ",";
  }
  else
    out << "...";
  out << "])";

  return out;
}

string Block::Serialize(){
  string ret = "";
  block_t header;

  header.timestamp = timestamp;
  header.size = size;
  header.index = index;
  header.leader = leader;
  header.parents_count = parents.size();
  header.tx_count = transactions.size();

  memcpy(header.groupId, groupId.data(), groupId.size());
  memcpy(header.hash, hash.data(), hash.size());

  ret = ret + string((char*) &header, sizeof(header));

  for (auto p : parents)
    ret = ret + p;
  for (auto t : transactions){
    ret = ret + t.Serialize();
  }
  return ret;
}

<<<<<<< Updated upstream
=======
// Other functions

bool Block::IsParent(Block &block){
  for (string p_hash : block.GetParents()){
    if(GetHash() == p_hash)
      return true;
  }
  return false;
}

bool Block::IsChild(Block &block) {
  return block.IsParent(*this);
}

bool Block::IsPartOfGroup(string groupId) {
  if(GetGroupId() == this->groupId)
    return true;
  else
    return false;
}

>>>>>>> Stashed changes
int Block::GetTxsCount(){
  return transactions.size();
}

<<<<<<< Updated upstream
=======
bool Block::IsMergeBlock() {
  if(GetParents().size() > 1)
    return true;
  else
    return false;
}

bool Block::IsMergeBlock (vector<string> parents) {

  for (auto &p : parents){
    bool found = false;
    for (auto &my_p : this->parents)
      if (p == my_p){
        found = true;
        break;
      }
    if (!found) return false;
  }
  return true;
}

string Block::CalculateHash(){
  hash = string(HASH_SIZE, 0);
  string serie = Serialize();
  // the hashing function can be found in utils
  hash = hashing(serie, HASH_SIZE);
  return hash;
}

>>>>>>> Stashed changes
int Block::CalculateSize(){
  int ret = CalculateHeaderSize();
  for (auto t : transactions)
    ret += t.GetSize();
  return ret;
}

int Block::CalculeTxsSize(){
  int ret = 0;
  for (auto t : transactions)
    ret += t.GetSize();
  return ret;
}

int Block::CalculateHeaderSize(){
  int ret = 0;
  ret += HASH_SIZE;
  ret += sizeof(size);
  ret += sizeof(index);
  ret += sizeof(leader);
  ret += sizeof(timestamp);
  ret += HASH_SIZE;
  ret += parents.size() * HASH_SIZE;

  return ret;
}
