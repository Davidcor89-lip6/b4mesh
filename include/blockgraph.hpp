/**
 * This file contains all the necessary enumerations and structs used throughout the simulation.
 * It also defines 2 very important classed; the Block and Blockgraph
 */
#ifndef BLOCKGRAPH_H
#define BLOCKGRAPH_H
#include <vector>
#include <map>
#include "block.hpp"

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
   * Adds a new block in the blockgraph if this block is valid
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

   string GetGroupId(string hash);


public:
   // funcitons related with blockgraph GROUPS
   /**
   * Gets the blocks from the groupId given
   */
  vector<Block> GetBlocksFromGroup (string groupId);


public:
  //  functions related with blockgraph branches
  /**
   * Gets the currents top blocks from blockgraph
   */
  //vector<string> GetChildlessBlocks ();
  vector<Block> GetChildlessBlocks();
  vector<string> GetChildlessBlockList ();
  
  /**
   *  Checks if the block given is a childlesblock
   */
  bool IsChildless (Block &block);

public:
  float MeanTxPerBlock();

  friend std::ostream& operator<< (std::ostream &out, const Blockgraph &blockgraph);
  Blockgraph& operator=(const Blockgraph& bg);

private:
  int totalBlocks;
  map<string, Block> blocks;  //vector contening all the blocks
  vector<int> meanTxBlock;

};

#endif
