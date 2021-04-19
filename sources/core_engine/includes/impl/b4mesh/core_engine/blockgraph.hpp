/**
 * This file contains all the necessary enumerations and structs used throughout the simulation.
<<<<<<< Updated upstream
 * It also defines 2 very important classed; the Block and Blockgraph
 */
#ifndef BLOCKGRAPH_H
#define BLOCKGRAPH_H

=======
 * It also defines the class Blockgraph
 */
#ifndef BLOCKGRAPH_H
#define BLOCKGRAPH_H
>>>>>>> Stashed changes
#include <vector>
#include <map>
#include <b4mesh/core_engine/block.hpp>

<<<<<<< Updated upstream
=======
#include <b4mesh/core_engine/transaction.hpp"

>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
  /**
   *  Get the size in bytes of the full blockgraph
   */
  int GetByteSize();
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
  /**
   *  Get the total number of transactions in the blockgraph
   */
  int GetTxsCount ();
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
  /**
   *  Get the size in bytes of all transactions in blockgraph
   */
  int GetTxsByteSize();
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
  /**
   *  Get total number of blocks
   */
  int GetBlocksCount () const;
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
  /**
   *  Get all the hashes in the blockgraph
   */
  vector<string> GetAllBlockHashes () const;
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
  /**
   *  Get all the blocks in the Blockgraph
   */
  const map<string, Block>& GetBlocks () const;

public:
  // functions related with blocks
<<<<<<< Updated upstream
  /**
   * Adds a new block in the blockgraph if this block is valid
   */
   void AddBlock (Block& newBlock);
=======

  /**
   * Adds a new block in the blockgraph
   */
   void AddBlock (Block& newBlock);

>>>>>>> Stashed changes
   /**
    * Check if the block given has been included in the blockgraph.
    */
   bool HasBlock (Block &block);
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
   /**
    * Checks if the block with the given hash
    * has been included in the blockgraph
    */
   bool HasBlock (string hash);
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
   /**
    * Return the block with the specified hash.
    * Should be called after hasBlock() to make sure that the block exists.
    */
   Block GetBlock (string hash);
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
   /**
    * Gets the children of a block
    */
   vector<string> GetChildren (Block &block);

<<<<<<< Updated upstream
   string GetGroupId(string hash);


public:
   // funcitons related with blockgraph GROUPS
=======

public:
   // funcitons related with blockgraph GROUPS

>>>>>>> Stashed changes
   /**
   * Gets the blocks from the groupId given
   */
  vector<Block> GetBlocksFromGroup (string groupId);

<<<<<<< Updated upstream

public:
  //  functions related with blockgraph branches
  /**
   * Gets the currents top blocks from blockgraph
   */
  //vector<string> GetChildlessBlocks ();
  vector<Block> GetChildlessBlocks();
=======
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
  
>>>>>>> Stashed changes
  /**
   *  Checks if the block given is a childlesblock
   */
  bool IsChildless (Block &block);

<<<<<<< Updated upstream
public:
=======
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
>>>>>>> Stashed changes
  float MeanTxPerBlock();

  friend std::ostream& operator<< (std::ostream &out, const Blockgraph &blockgraph);
  Blockgraph& operator=(const Blockgraph& bg);

private:
<<<<<<< Updated upstream
  int totalBlocks;
  map<string, Block> blocks;  //vector contening all the blocks
  vector<int> meanTxBlock;

=======

  map<string, Block> blocks;  // The blockgraph < hash, block >
  int txsCount;
  int txsSize;
  vector<int> meanTxBlock;  
>>>>>>> Stashed changes
};

#endif
