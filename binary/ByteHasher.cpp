
#include "ByteHasher.h"
#include "../hashing/SHA256.h"

#include <string>
#include <vector>

// Generates SHA-256 hashes for each binary block.
// Preserves all bytes, including null bytes, during hashing.
// Returns a vector containing the hash of each block.
    std::vector<std::string> hashByteBlocks(
    const std::vector<std::vector<std::uint8_t>>& blocks)
{
    std::vector<std::string> hashes;
    hashes.reserve(blocks.size());

    for (const auto& block : blocks)
    {
        // Preserve all bytes, including null bytes.
        const std::string data(
            reinterpret_cast<const char*>(block.data()),
            block.size()
        );

        hashes.push_back(generateSHA256(data));
    }

    return hashes;
}
