
#ifndef MERKLE_TREE_H
#define MERKLE_TREE_H

#include "MerkleNode.h"
#include "HashGenerator.h"

#include <string>
#include <vector>
#include <memory>
#include <cstddef>

class MerkleTree {

private:
    std::unique_ptr<MerkleNode> root;

    HashGenerator hashGenerator;

    size_t leafCount;

    bool isValidDigest(const std::string& digest) const;

public:
    MerkleTree();

    bool buildTree(
        const std::vector<std::string>& blockHashes
    );

    const std::string& getRootHash() const;

    const MerkleNode* getRoot() const;

    size_t getLeafCount() const;

    void clear();
};

#endif
