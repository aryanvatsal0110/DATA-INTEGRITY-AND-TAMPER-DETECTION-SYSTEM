
#include "MerkleTree.h"

#include <limits>
#include <utility>
#include <vector>


MerkleTree::MerkleTree() : leafCount(0) {
}


bool MerkleTree::isValidDigest(
    const std::string& digest
) const {

    if (digest.size() != 64) {
        return false;
    }

    for (unsigned char ch : digest) {

        if (!((ch >= '0' && ch <= '9') ||
              (ch >= 'a' && ch <= 'f') ||
              (ch >= 'A' && ch <= 'F'))) {

            return false;
        }
    }

    return true;
}


void MerkleTree::clear() {

    root.reset();
    leafCount = 0;
}


bool MerkleTree::buildTree(
    const std::vector<std::string>& blockHashes
) {

    clear();

    if (blockHashes.empty()) {
        return false;
    }

    for (const std::string& hash : blockHashes) {

        if (!isValidDigest(hash)) {
            return false;
        }
    }

    // Find next power of two
    size_t targetSize = 1;

    while (targetSize < blockHashes.size()) {

        if (targetSize >
            std::numeric_limits<size_t>::max() / 2) {
            return false;
        }

        targetSize *= 2;
    }

    leafCount = blockHashes.size();

    std::vector<std::unique_ptr<MerkleNode>> currentLevel;

    currentLevel.reserve(targetSize);

    // Create leaf nodes
    for (size_t i = 0; i < blockHashes.size(); i++) {

        std::string leafInput(1, '\0');

        leafInput += blockHashes[i];

        std::string leafHash;

        if (!hashGenerator.generateHash(leafInput, leafHash)) {
            clear();
            return false;
        }

        currentLevel.push_back(
            std::make_unique<MerkleNode>(
                leafHash,
                true,
                i,
                false
            )
        );
    }

    // Duplicate final leaf when required
    while (currentLevel.size() < targetSize) {

        currentLevel.push_back(
            std::make_unique<MerkleNode>(
                currentLevel.back()->hash,
                true,
                blockHashes.size() - 1,
                true
            )
        );
    }

    // Construct parent levels
    while (currentLevel.size() > 1) {

        std::vector<std::unique_ptr<MerkleNode>> nextLevel;

        nextLevel.reserve(currentLevel.size() / 2);

        for (size_t i = 0; i < currentLevel.size(); i += 2) {

            std::string parentInput(1, '\1');

            parentInput += currentLevel[i]->hash;
            parentInput += currentLevel[i + 1]->hash;

            std::string parentHash;

            if (!hashGenerator.generateHash(
                    parentInput,
                    parentHash
                )) {

                clear();
                return false;
            }

            auto parent = std::make_unique<MerkleNode>(
                parentHash,
                false
            );

            parent->left = std::move(currentLevel[i]);

            parent->right = std::move(currentLevel[i + 1]);

            nextLevel.push_back(std::move(parent));
        }

        currentLevel = std::move(nextLevel);
    }

    root = std::move(currentLevel[0]);

    return true;
}


const std::string& MerkleTree::getRootHash() const {

    static const std::string emptyHash;

    if (!root) {
        return emptyHash;
    }

    return root->hash;
}


const MerkleNode* MerkleTree::getRoot() const {

    return root.get();
}


size_t MerkleTree::getLeafCount() const {

    return leafCount;
}
