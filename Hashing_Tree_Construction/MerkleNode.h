
#ifndef MERKLE_NODE_H
#define MERKLE_NODE_H

#include <string>
#include <memory>
#include <cstddef>

class MerkleNode {
public:
    std::string hash;

    bool isLeaf;
    bool isDuplicate;

    size_t blockIndex;

    std::unique_ptr<MerkleNode> left;
    std::unique_ptr<MerkleNode> right;

    MerkleNode(
        const std::string& nodeHash,
        bool leaf,
        size_t index = 0,
        bool duplicate = false
    )
        : hash(nodeHash),
          isLeaf(leaf),
          isDuplicate(duplicate),
          blockIndex(index) {
    }
};

#endif
