
#include "Partitioner.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

std::vector<Block> adaptivePartition(
    const std::vector<std::string>& records,
    const DataProfile& profile)
{
    std::vector<Block> blocks;

    if (records.empty())
        return blocks;
    int blockSize;

    if (profile.recordCount <= 100)
        blockSize = 10;
    else if (profile.recordCount <= 1000)
        blockSize = 50;
    else
        blockSize = 100;

    int blockID = 0;

    for (std::size_t start = 0;
         start < records.size();
         start += static_cast<std::size_t>(blockSize))
    {
        Block block;
        block.blockID = blockID++;

        const std::size_t end = std::min(
            start + static_cast<std::size_t>(blockSize),
            records.size()
        );

        for (std::size_t i = start; i < end; ++i)
            block.records.push_back(records[i]);

        blocks.push_back(block);
    }

    return blocks;
}

void displayBlocks(const std::vector<Block>& blocks)
{
    std::cout << "\n--- Partitioning Results ---\n";
    std::cout << "Total blocks: " << blocks.size() << '\n';

    for (const Block& block : blocks)
    {
        std::cout << "Block ID: " << block.blockID
                  << " | Records: " << block.records.size()
                  << '\n';
    }
}
