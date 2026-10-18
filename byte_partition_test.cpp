
#include "input/InputHandler.h"
#include "binary/BytePartitioner.h"

#include <iostream>
#include <numeric>

int main()
{
    const auto bytes = readFileBytes("data/sample.txt");

    if (bytes.empty())
    {
        std::cerr << "FAIL: Could not read sample file or file is empty.\n";
        return 1;
    }

    const std::size_t blockSize = 64;
    const auto blocks = partitionBytes(bytes, blockSize);

    std::size_t reconstructedSize = 0;

    for (const auto& block : blocks)
    {
        reconstructedSize += block.size();
    }

    std::cout << "Original bytes: " << bytes.size() << '\n';
    std::cout << "Block size: " << blockSize << '\n';
    std::cout << "Number of blocks: " << blocks.size() << '\n';
    std::cout << "Reconstructed size: " << reconstructedSize << '\n';

    if (reconstructedSize != bytes.size())
    {
        std::cerr << "FAIL: Byte count changed during partitioning.\n";
        return 1;
    }

    std::vector<std::uint8_t> reconstructed;

    for (const auto& block : blocks)
    {
        reconstructed.insert(
            reconstructed.end(), block.begin(), block.end()
        );
    }

    if (reconstructed != bytes)
    {
        std::cerr << "FAIL: Reconstructed bytes differ from input.\n";
        return 1;
    }

    std::cout << "PASS: All bytes preserved across blocks.\n";
    return 0;
}
