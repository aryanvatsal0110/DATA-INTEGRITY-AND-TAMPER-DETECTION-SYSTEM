#ifndef BINARY_MERKLE_TREE_H
#define BINARY_MERKLE_TREE_H

#include <string>
#include <vector>

#include "MerkleNode.h"
#include "Block.h"

class BinaryMerkleTree
{
private:

    MerkleNode* root;

    std::string hashBlock(
        const Block& block
    );

    MerkleNode* buildTree(
        const std::vector<std::string>& hashes,
        int start,
        int end
    );

    void deleteTree(
        MerkleNode* node
    );

public:

    BinaryMerkleTree();

    ~BinaryMerkleTree();

    void build(
        const std::vector<Block>& blocks
    );

    MerkleNode* getRoot() const;

    std::string getRootHash() const;
};

#endif