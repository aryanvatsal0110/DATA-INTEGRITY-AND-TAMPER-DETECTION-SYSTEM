
#include "BytePartitioner.h"

#include <stdexcept>

std::vector<std::vector<std::uint8_t>> partitionBytes(
    const std::vector<std::uint8_t>& bytes,
    std::size_t blockSize)
{
    if (blockSize == 0)
    {
        throw std::invalid_argument("Block size must be greater than zero.");
    }

    std::vector<std::vector<std::uint8_t>> blocks;

    for (std::size_t start = 0; start < bytes.size(); start += blockSize)
    {
        const std::size_t length =
            (bytes.size() - start < blockSize)
                ? bytes.size() - start
                : blockSize;

        blocks.emplace_back(
            bytes.begin() + start,
            bytes.begin() + start + length
        );
    }

    return blocks;
}
