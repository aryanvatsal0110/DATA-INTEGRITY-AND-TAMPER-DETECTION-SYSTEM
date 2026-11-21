
#ifndef HIERARCHICAL_MERKLE_TREE_H
#define HIERARCHICAL_MERKLE_TREE_H

#include "MerkleTree.h"

#include <string>
#include <vector>
#include <cstddef>

class HierarchicalMerkleTree {

private:
    std::vector<MerkleTree> subtrees;

    MerkleTree globalTree;

    std::vector<std::string> subtreeRoots;

    size_t originalLeafCount;
    size_t blocksPerSubtree;

public:
    HierarchicalMerkleTree();

    bool buildTree(
        const std::vector<std::string>& blockHashes,
        size_t groupSize = 1000
    );

    const std::string& getGlobalRootHash() const;

    const std::string& getSubtreeRoot(size_t index) const;

    const MerkleTree* getSubtree(size_t index) const;

    size_t getSubtreeCount() const;

    size_t getLeafCount() const;

    size_t getBlocksPerSubtree() const;

    void clear();
};

#endif
