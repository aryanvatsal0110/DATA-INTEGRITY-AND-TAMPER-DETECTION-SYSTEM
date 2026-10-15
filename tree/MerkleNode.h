#ifndef MERKLE_NODE_H
#define MERKLE_NODE_H

#include <string>

struct MerkleNode
{
    std::string hash;

    MerkleNode* left;
    MerkleNode* right;

    MerkleNode(const std::string& hashValue)
    {
        hash = hashValue;
        left = nullptr;
        right = nullptr;
    }
};

#endif