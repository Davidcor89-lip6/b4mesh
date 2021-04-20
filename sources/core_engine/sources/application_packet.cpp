#include <b4mesh/core_engine/application_packet.hpp>

ApplicationPacket::ApplicationPacket(){
  service = NONE;
  payload = "";
  size = CalculateSize();
}

ApplicationPacket::ApplicationPacket(char service, string payload){
  this->service = service;
  this->payload = payload;
  size = CalculateSize();
}

ApplicationPacket::ApplicationPacket(char service, int payload_size, 
                                     const uint8_t* payload){
  this->service = service;
  this->payload = string((const char*) payload, payload_size);
  size = CalculateSize();
}

ApplicationPacket::ApplicationPacket(ApplicationPacket &p){
  service = p.service;
  payload = p.payload;
  size = p.size;
}

// Packet deserialization
ApplicationPacket::ApplicationPacket(string &serie){
  packet_hdr_serie *hdr = (packet_hdr_serie*) serie.data();
  service = hdr->service;
  size = hdr->size;
  const char* packet_payload = serie.data() + HeaderSize();
  payload = string(packet_payload, size-HeaderSize());
}

ApplicationPacket::ApplicationPacket(string &serie, std::size_t p_size ){
  packet_hdr_serie *hdr = (packet_hdr_serie*) serie.data();
  service = hdr->service;
  size = hdr->size;

  if (size == p_size)
  {
    const char* packet_payload = serie.data() + HeaderSize();
    payload = string(packet_payload, size-HeaderSize());
  } else {
    std::cout << " ApplicationPacket : " << service << " : " << size << "/" << p_size << std::endl;
    service = NONE;
    payload = "";
    size = CalculateSize();
  }
}

ApplicationPacket::~ApplicationPacket(){
}

int ApplicationPacket::CalculateSize(){
  return HeaderSize() + payload.size();
}

int ApplicationPacket::HeaderSize(){
  return sizeof(char) + sizeof(int);
}

void ApplicationPacket::SetSize(int size){
  this->size = size;
}

int ApplicationPacket::GetSize(){
  return size;
}

void ApplicationPacket::SetService(char service){
  this->service = service;
}

char ApplicationPacket::GetService(){
  return service;
}

void ApplicationPacket::SetPayload(string payload){
  this->payload = payload;
}

string ApplicationPacket::GetPayload(){
  return payload;
}

string ApplicationPacket::Serialize(){
  string ret(HeaderSize(), 0);
  packet_hdr_serie* p = (packet_hdr_serie*) ret.data();

  p->service = service;
  p->size = size;
  
  ret = ret + payload;
  return ret;
}

ostream& operator<<(ostream& o, const ApplicationPacket& p){
  o << "Packet(" << (int)p.service << "," << p.size << "," << p.payload << ")";
  return o;
}
