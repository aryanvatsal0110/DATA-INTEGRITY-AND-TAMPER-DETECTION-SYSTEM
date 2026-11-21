
#include "RootManager.h"

RootManager::RootManager() : hasMetadata(false) {
}


// Validate SHA-256 hexadecimal hash
bool RootManager::isValidHash(
    const std::string& hash
) const {

    if (hash.size() != 64) {
        return false;
    }

    for (unsigned char ch : hash) {

        if (!((ch >= '0' && ch <= '9') ||
              (ch >= 'a' && ch <= 'f') ||
              (ch >= 'A' && ch <= 'F'))) {

            return false;
        }
    }

    return true;
}


// Store original root metadata
bool RootManager::setRootMetadata(
    const std::string& rootHash,
    size_t fileSize,
    size_t blockSize,
    size_t totalBlocks,
    TreeType treeType,
    size_t groupSize
) {

    clear();

    // Validate root hash
    if (!isValidHash(rootHash)) {
        return false;
    }

    // Validate partition information
    if (blockSize == 0 || totalBlocks == 0) {
        return false;
    }

    // Validate tree configuration
    if (treeType == TreeType::STANDARD) {

        if (groupSize != 0) {
            return false;
        }

    }
    else if (treeType == TreeType::HIERARCHICAL) {

        if (groupSize == 0) {
            return false;
        }

    }
    else {
        return false;
    }

    // Store metadata
    metadata.merkleRoot = rootHash;

    metadata.originalFileSize = fileSize;

    metadata.blockSize = blockSize;

    metadata.totalBlocks = totalBlocks;

    metadata.treeType = treeType;

    metadata.groupSize = groupSize;

    hasMetadata = true;

    return true;
}


// Validate stored metadata
bool RootManager::validateMetadata() const {

    if (!hasMetadata) {
        return false;
    }

    if (!isValidHash(metadata.merkleRoot)) {
        return false;
    }

    if (metadata.blockSize == 0 ||
        metadata.totalBlocks == 0) {

        return false;
    }

    if (metadata.treeType == TreeType::STANDARD) {

        return metadata.groupSize == 0;
    }

    if (metadata.treeType == TreeType::HIERARCHICAL) {

        return metadata.groupSize > 0;
    }

    return false;
}


// Return metadata without allowing modification
const RootMetadata* RootManager::getMetadata() const {

    if (!hasMetadata) {
        return nullptr;
    }

    return &metadata;
}


// Check whether metadata exists
bool RootManager::hasRootMetadata() const {

    return hasMetadata;
}


// Clear stored metadata
void RootManager::clear() {

    metadata = RootMetadata();

    hasMetadata = false;
}
