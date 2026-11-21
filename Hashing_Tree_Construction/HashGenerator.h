
#ifndef HASH_GENERATOR_H
#define HASH_GENERATOR_H

#include "Block.h"

#include <string>
#include <vector>

class HashGenerator {
public:

    // Generate SHA-256 hash of a single block
    bool generateHash(
        const std::string& data,
        std::string& hash
    ) const;

    // Generate SHA-256 hashes for all blocks
    bool generateBlockHashes(
        const std::vector<Block>& blocks,
        std::vector<std::string>& hashes
    ) const;
};

#endif
