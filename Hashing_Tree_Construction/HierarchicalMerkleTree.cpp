
#include "HierarchicalMerkleTree.h"

#include <algorithm>
#include <utility>


HierarchicalMerkleTree::HierarchicalMerkleTree()
    : originalLeafCount(0),
      blocksPerSubtree(0) {
}


// Clear the existing hierarchical tree
void HierarchicalMerkleTree::clear() {

    subtrees.clear();

    subtreeRoots.clear();

    globalTree.clear();

    originalLeafCount = 0;

    blocksPerSubtree = 0;
}


// Build the hierarchical Merkle Tree
bool HierarchicalMerkleTree::buildTree(
    const std::vector<std::string>& blockHashes,
    size_t groupSize
) {

    clear();

    // Validate input
    if (blockHashes.empty() || groupSize == 0) {
        return false;
    }

    originalLeafCount = blockHashes.size();

    blocksPerSubtree = groupSize;

    // Calculate number of subtrees without overflow
    const size_t totalSubtrees =
        blockHashes.size() / groupSize +
        (blockHashes.size() % groupSize != 0);

    subtrees.reserve(totalSubtrees);

    subtreeRoots.reserve(totalSubtrees);

    // Step 1: Construct individual subtrees
    for (size_t start = 0;
         start < blockHashes.size(); ) {

        const size_t currentGroupSize =
            std::min(groupSize, blockHashes.size() - start);

        std::vector<std::string> groupHashes;

        groupHashes.reserve(currentGroupSize);

        for (size_t i = 0; i < currentGroupSize; i++) {

            groupHashes.push_back(blockHashes[start + i]);
        }

        // Build one Merkle Tree for this group
        MerkleTree subtree;

        if (!subtree.buildTree(groupHashes)) {

            clear();

            return false;
        }

        // Save the subtree root
        subtreeRoots.push_back(subtree.getRootHash());

        // Store the complete subtree
        subtrees.push_back(std::move(subtree));

        start += currentGroupSize;
    }


    // Step 2: Construct global tree using subtree roots
    if (!globalTree.buildTree(subtreeRoots)) {

        clear();

        return false;
    }

    return true;
}


// Return global Merkle Root
const std::string&
HierarchicalMerkleTree::getGlobalRootHash() const {

    return globalTree.getRootHash();
}


// Return root of a particular subtree
const std::string&
HierarchicalMerkleTree::getSubtreeRoot(size_t index) const {

    static const std::string emptyHash;

    if (index >= subtreeRoots.size()) {
        return emptyHash;
    }

    return subtreeRoots[index];
}


// Return a particular subtree
const MerkleTree*
HierarchicalMerkleTree::getSubtree(size_t index) const {

    if (index >= subtrees.size()) {
        return nullptr;
    }

    return &subtrees[index];
}


// Return number of subtrees
size_t HierarchicalMerkleTree::getSubtreeCount() const {

    return subtrees.size();
}


// Return original number of block hashes
size_t HierarchicalMerkleTree::getLeafCount() const {

    return originalLeafCount;
}


// Return configured group size
size_t HierarchicalMerkleTree::getBlocksPerSubtree() const {

    return blocksPerSubtree;
}
