
#include "TamperLocalizer.h"
#include "../hashing/SHA256.h"

#include <cstddef>
#include <string>
#include <vector>

std::vector<int> locateTamperedBlocks(
    const std::vector<Block>& blocks,
    const std::vector<std::string>& trustedLeafHashes)
{
    std::vector<int> affectedBlocks;

    // A size mismatch means the block layout has changed.
    if (blocks.size() != trustedLeafHashes.size())
    {
        affectedBlocks.push_back(-1);
        return affectedBlocks;
    }

    for (std::size_t i = 0; i < blocks.size(); ++i)
    {
        std::string combinedData;

        for (const std::string& record : blocks[i].records)
        {
            /* // Length-prefix records to avoid ambiguous concatenation.
            combinedData += std::to_string(record.size());
            combinedData += ':';
            combinedData += record; */
            combinedData += record;
        }

        const std::string currentHash =
            generateSHA256(combinedData);

        if (currentHash != trustedLeafHashes[i])
        {
            affectedBlocks.push_back(blocks[i].blockID);
        }
    }

    return affectedBlocks;
}
