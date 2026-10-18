
#ifndef BYTE_HASHER_H
#define BYTE_HASHER_H

#include <cstdint>
#include <string>
#include <vector>

// Generate one SHA-256 hash for each binary block.
std::vector<std::string> hashByteBlocks(
    const std::vector<std::vector<std::uint8_t>>& blocks
);

#endif
