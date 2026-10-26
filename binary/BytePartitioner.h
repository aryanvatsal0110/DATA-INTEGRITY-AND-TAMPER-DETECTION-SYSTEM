
#ifndef BYTE_PARTITIONER_H
#define BYTE_PARTITIONER_H

#include <cstddef>
#include <cstdint>
#include <vector>

// Divide raw file bytes into blocks of the requested size.
// The final block may be smaller than blockSize.
std::vector<std::vector<std::uint8_t>> partitionBytes(
    const std::vector<std::uint8_t>& bytes,
    std::size_t blockSize
);

#endif
