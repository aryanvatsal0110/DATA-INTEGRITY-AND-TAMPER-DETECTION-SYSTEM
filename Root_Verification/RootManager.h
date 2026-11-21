
#ifndef ROOT_MANAGER_H
#define ROOT_MANAGER_H

#include <string>
#include <cstddef>

enum class TreeType {
    STANDARD,
    HIERARCHICAL
};

struct RootMetadata {

    std::string merkleRoot;

    size_t originalFileSize = 0;

    size_t blockSize = 0;

    size_t totalBlocks = 0;

    TreeType treeType = TreeType::STANDARD;

    size_t groupSize = 0;
};

class RootManager {

private:

    RootMetadata metadata;

    bool hasMetadata;

    bool isValidHash(const std::string& hash) const;

public:

    RootManager();

    bool setRootMetadata(
        const std::string& rootHash,
        size_t fileSize,
        size_t blockSize,
        size_t totalBlocks,
        TreeType treeType,
        size_t groupSize = 0
    );

    bool validateMetadata() const;

    const RootMetadata* getMetadata() const;

    bool hasRootMetadata() const;

    void clear();
};

#endif
