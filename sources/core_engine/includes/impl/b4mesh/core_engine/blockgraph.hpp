/**
 * This file contains all the necessary enumerations and structs used throughout the simulation.
 * It also defines the class Blockgraph
 */
#ifndef BLOCKGRAPH_H
#define BLOCKGRAPH_H
#include <vector>
#include <map>
#include <b4mesh/core_engine/block.hpp>
#include <b4mesh/core_engine/transaction.hpp>

using namespace std;

class Blockgraph
{
public:
  Blockgraph ();
  Blockgraph (const Blockgraph& b);
  Blockgraph (char* blockgraph_serialized);
  virtual ~Blockgraph();

public:
  //Blockgraph functions

  /**
   *  Get the size in bytes of the full blockgraph
   */
  int GetByteSize();

  /**
   *  Get the total number of transactions in the blockgraph
   */
  int GetTxsCount ();

  /**
   *  Get the size in bytes of all transactions in blockgraph
   */
  int GetTxsByteSize();

  /**
   *  Get total number of blocks
   */
  int GetBlocksCount () const;

  /**
   *  Get all the hashes in the blockgraph
   */
  vector<string> GetAllBlockHashes () const;

  /**
   *  Get all the blocks in the Blockgraph
   */
  const map<string, Block>& GetBlocks () const;

public:
  // functions related with blocks

  /**
   * Adds a new block in the blockgraph
   */
   void AddBlock (Block& newBlock);

   /**
    * Check if the block given has been included in the blockgraph.
    */
   bool HasBlock (Block &block);

   /**
    * Checks if the block with the given hash
    * has been included in the blockgraph
    */
   bool HasBlock (string hash);

   /**
    * Return the block with the specified hash.
    * Should be called after hasBlock() to make sure that the block exists.
    */
   Block GetBlock (string hash);

   /**
    * Gets the children of a block
    */
   vector<string> GetChildren (Block &block);


public:
   // funcitons related with blockgraph GROUPS

   /**
   * Gets the blocks from the groupId given
   */
  vector<Block> GetBlocksFromGroup (string groupId);

   /**
    * Gets the block's groupId
    */
   string GetGroupId(string hash);



public:
  //  functions related with blockgraph branches

  /**
   * Gets the currents top blocks from blockgraph
   */
  vector<Block> GetChildlessBlocks();

  /**
   * Gets a list of the currents top blocks from blockgraph
   */
  vector<string> GetChildlessBlockList ();
  
  /**
   *  Checks if the block given is a childlesblock
   */
  bool IsChildless (Block &block);

  bool IsTxInBG (Transaction &t);

  /**
   *  Count the number of occurance of a transaction in the blockgraph
   */
  int CountRepTxInBlockGraph (Transaction &t);

  int ComputeTransactionRepetition (void);

public:
  // performances functions 

   /**
   * Calculates the mean number of transactions per block
   * in all the blockgraph
   */
  float MeanTxPerBlock();

  friend std::ostream& operator<< (std::ostream &out, const Blockgraph &blockgraph);
  Blockgraph& operator=(const Blockgraph& bg);

private:

  map<string, Block> blocks;  // The blockgraph < hash, block >
  int txsCount;
  int txsSize;
  vector<int> meanTxBlock;  
};

#endif
