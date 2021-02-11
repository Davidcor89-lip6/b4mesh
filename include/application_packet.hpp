#ifndef APPLICATION_PACKET_H
#define APPLICATION_PACKET_H
#include <iostream>
#include <string>

using namespace std;

class ApplicationPacket{
  public:
    // Typedef
    typedef struct packet_hdr_serie{
      int size;
      char service;
    } packet_hdr_serie;

  public:
    // Constants
    enum {NONE, RAFT, PBFT, BLOCKGRAPH, RAFTB4M, TRANSACTION, BLOCK, REQUEST_BLOCK, CHANGE_TOPO};
  public:
    //Constructors and destructor
    ApplicationPacket();
    ApplicationPacket(char service, string payload);
    ApplicationPacket(char service, int size, const uint8_t* payload);
    ApplicationPacket(ApplicationPacket &p);
    ApplicationPacket(string &serie);
    ApplicationPacket(string &serie, int p_size);
    ~ApplicationPacket();

  public:
    // Getters, Setters and Operators;
    void SetSize(int size);
    int GetSize();
    void SetService(char service);
    char GetService();
    void SetPayload(string payload);
    string GetPayload();

    ostream& operator<<(const ostream& o);
    friend ostream& operator<<(ostream& o, const ApplicationPacket& p);
  public:
    /**
     * Calculate and return the total size of the packet
     */
    int CalculateSize();

    /**
     * Calculate and return the size of the header of the packet
     */
    int HeaderSize();

    /**
     * Serialize the packet into an array of byte (here a string is more
     * convenient)
     */
     string Serialize();

  private:
    char service; // Service identifier of the packet (PBFT, RAFT, BLOCKCHAIN, ...)
    int size; // Size of the whole packet (including header)
    string payload; // Payload of the packet (contains an application message

};

#endif
