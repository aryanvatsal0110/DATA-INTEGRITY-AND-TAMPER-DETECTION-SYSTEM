#ifndef HIERARCHICAL_MERKLE_TREE_H
#define HIERARCHICAL_MERKLE_TREE_H

#include <string>
#include <vector>

#include "MerkleNode.h"
#include "Block.h"

class HierarchicalMerkleTree
{
private:

    MerkleNode* root;

    int blocksPerSubtree;

    std::string hashBlock(
        const Block& block
    );

    MerkleNode* buildSubtree(
        const std::vector<Block>& blocks,
        int start,
        int end
    );

    MerkleNode* buildTopLevelTree(
        const std::vector<MerkleNode*>& subtreeRoots,
        int start,
        int end
    );

    void deleteTree(
        MerkleNode* node
    );

public:

    HierarchicalMerkleTree(
        int subtreeSize = 4
    );

    ~HierarchicalMerkleTree();

    void build(
        const std::vector<Block>& blocks
    );

    MerkleNode* getRoot() const;

    std::string getRootHash() const;
};

#endif