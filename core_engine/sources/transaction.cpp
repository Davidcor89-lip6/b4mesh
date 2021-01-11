#include "transaction.hpp"

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

}

bool Transaction::operator== (const Transaction &tx){
  return hash == tx.hash;
}

ostream& operator<< (std::ostream &out, const Transaction &tx){
  out << "Transaction(" << dump(tx.hash.data(), 10) << "," << tx.timestamp << "," << tx.size << ","
    << dump(tx.payload.data(), 10) << ")";
  return out;
}

int Transaction::CalculateSize(){
  size = CalculateHeaderSize() + payload.size();
  return size;
}
int Transaction::CalculateHeaderSize(){
  return HASH_SIZE + sizeof(size) + sizeof(timestamp);
}

string Transaction::CalculateHash(){
  hash = string(HASH_SIZE, 0);
  string serie = Serialize();
  hash = hashing(serie, HASH_SIZE);
  return hash;
}
