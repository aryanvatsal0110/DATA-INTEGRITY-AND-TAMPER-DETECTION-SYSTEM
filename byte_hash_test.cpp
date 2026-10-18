
#include "input/InputHandler.h"
#include "binary/BytePartitioner.h"
#include "binary/ByteHasher.h"

#include <iostream>
#include <string>

int main()
{
    const auto bytes = readFileBytes("data/sample.txt");

    if (bytes.empty())
    {
        std::cerr << "FAIL: File is empty or could not be read.\n";
        return 1;
    }

    const auto blocks = partitionBytes(bytes, 64);
    const auto hashes = hashByteBlocks(blocks);

    if (hashes.size() != blocks.size())
    {
        std::cerr << "FAIL: Block and hash counts differ.\n";
        return 1;
    }

    for (std::size_t i = 0; i < hashes.size(); ++i)
    {
        if (hashes[i].size() != 64)
        {
            std::cerr << "FAIL: Invalid SHA-256 hash length.\n";
            return 1;
        }

        std::cout << "Block " << i
                  << " (" << blocks[i].size() << " bytes): "
                  << hashes[i] << '\n';
    }

    std::cout << "PASS: Every block has a SHA-256 hash.\n";
    return 0;
}
