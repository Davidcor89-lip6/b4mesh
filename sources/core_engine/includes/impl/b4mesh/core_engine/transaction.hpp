#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <string.h>
#include <b4mesh/utils/utils.hpp>

using namespace std;

class Transaction{

  public:
    static const int HASH_SIZE = 20;

  public:
    //typedef of a transaction
    typedef struct transaction_t{
      double    timestamp;
      int       size;
      char    hash[HASH_SIZE];
    } tx_t;

  public:
    //constructor and destructor
    Transaction();
    Transaction (string hash, int size, string payload, double timestamp);
    Transaction (const Transaction &tx);
    Transaction (const string& serie);
    ~Transaction();

  public:
    //Getters, Setters & Operators
    string GetHash (void);
    void SetHash (string hash);

    int GetSize (void);
    void SetSize (int size);

    string GetPayload (void);
    void SetPayload (string payload);

    double GetTimestamp (void);
    void SetTimestamp (double timestamp);

    bool operator==(const Transaction &tx);
    friend std::ostream& operator<< (std::ostream &out, const Transaction &tx);
    Transaction& operator=(const Transaction &tx2);

  public:
    /**
     * Serialize the transaction into an array of bits 
     */
    string Serialize();
    /**
     * Calculate and return the total size of the transaction
     */
    int CalculateSize();
    /**
     * Calculate and return the total size of the transaction header 
     */
    int CalculateHeaderSize();
    /**
     * Calculate and return the hash of the transaction
     */
    string CalculateHash();

  public:
    // Other functions

  private:

    string    hash;           // Hash of the transaction (also an identifier)
    int       size;           // Size of the transaction
    double    timestamp;      // Time when a transaction is created
    string    payload;        // data of the transaction
};

#endif
