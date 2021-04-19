<<<<<<< Updated upstream
=======
/*
  The class transaction implements the definition of a transaction 
  which is sent to all nodes belonging to the same group as the node creating it.
*/

>>>>>>> Stashed changes
#include <b4mesh/core_engine/transaction.hpp>

Transaction::Transaction(){
  hash = string(HASH_SIZE, 0);
  size = 0;
  payload = "";
  timestamp = 0.0;

  CalculateSize();
  CalculateHash();
}

Transaction::Transaction (string hash, int size,
                          string payload, double timestamp){
  this->hash =         hash;
  this->size =         size;
  this->payload =      payload;
  this->timestamp = timestamp;

  CalculateSize();
  CalculateHash();
}

Transaction::Transaction (const Transaction &tx){
  hash =         tx.hash;
  size =         tx.size;
  payload =      tx.payload;
  timestamp = tx.timestamp;
}

<<<<<<< Updated upstream
Transaction::Transaction(const string &serie){
  const char *p = serie.data();
  transaction_t* p_header = (transaction_t*)p;

  hash = string(p_header->hash, HASH_SIZE);
  size = p_header->size;
  timestamp = p_header->timestamp;
  int payload_size = size - sizeof(transaction_t);
  payload = string(p+sizeof(transaction_t), payload_size);

}
Transaction::~Transaction (){

=======
// Transaction deserialization
Transaction::Transaction(const string &serie){
  try
  {
    const char *p = serie.data();
    transaction_t* p_header = (transaction_t*)p;
    hash = string(p_header->hash, HASH_SIZE);
    size = p_header->size;
    timestamp = p_header->timestamp;;
    int payload_size = size - sizeof(transaction_t);
    payload = string(p+sizeof(transaction_t), payload_size);
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    std::cout << "Error in the deserialization of a transaction" << std::endl;
    throw e;
  }
}

Transaction::~Transaction (){
>>>>>>> Stashed changes
}

string Transaction::GetHash (){
  return hash;
}

void Transaction::SetHash(string hash){
  this->hash = hash;
}

int Transaction::GetSize (){
  CalculateSize();
  return size;
}

void Transaction::SetSize (int size){
  this->size = size;
  CalculateHash();
}

string Transaction::GetPayload (){
  return payload;
}

void Transaction::SetPayload (string payload){
  this->payload = payload;
  CalculateSize();
  CalculateHash();
}

double Transaction::GetTimestamp (){
  return timestamp;
}

void Transaction::SetTimestamp (double timestamp){
  this->timestamp = timestamp;
  CalculateHash();
}

Transaction& Transaction::operator=(const Transaction &tx2){
  hash = tx2.hash;
  size = tx2.size;
  payload = tx2.payload;
  timestamp = tx2.timestamp;

  return *this;
}

string Transaction::Serialize(){
  string ret = "";
  transaction_t header;

  header.timestamp = timestamp;
  header.size = size;
  memcpy(header.hash, hash.data(), HASH_SIZE);
  ret = ret + string((char*)&header, sizeof(header));

  ret = ret + payload;
  return ret;
<<<<<<< Updated upstream

=======
>>>>>>> Stashed changes
}

bool Transaction::operator== (const Transaction &tx){
  return hash == tx.hash;
}

ostream& operator<< (std::ostream &out, const Transaction &tx){
  out << "Transaction(" << dump(tx.hash.data(), 10) << "," << tx.timestamp << "," << tx.size << ","
<<<<<<< Updated upstream
    << dump(tx.payload.data(), 10) << ")";
=======
      << dump(tx.payload.data(), 10) << ")";
>>>>>>> Stashed changes
  return out;
}

int Transaction::CalculateSize(){
  size = CalculateHeaderSize() + payload.size();
  return size;
}
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
int Transaction::CalculateHeaderSize(){
  return HASH_SIZE + sizeof(size) + sizeof(timestamp);
}

string Transaction::CalculateHash(){
  hash = string(HASH_SIZE, 0);
  string serie = Serialize();
<<<<<<< Updated upstream
=======
  // the hashing function is found in utils.cpp
>>>>>>> Stashed changes
  hash = hashing(serie, HASH_SIZE);
  return hash;
}
