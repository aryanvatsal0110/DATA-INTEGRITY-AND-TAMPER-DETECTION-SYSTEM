
#include "Partitioner.h"

#include <stdexcept>

size_t Partitioner::selectBlockSize(size_t fileSize) const {

    constexpr size_t KB = 1024;
    constexpr size_t MB = 1024 * KB;

    if (fileSize <= 4 * KB) {
        return 512;
    }
    else if (fileSize <= MB) {
        return 4 * KB;
    }
    else if (fileSize <= 100 * MB) {
        return 64 * KB;
    }
    else {
        return 256 * KB;
    }
}


void Partitioner::partitionFile(
    const std::string& fileData,
    std::vector<Block>& blocks,
    PartitionConfig& config
) const {

    // Clear previous partition results
    blocks.clear();

    const size_t fileSize = fileData.size();

    // Select adaptive block size
    config.blockSize = selectBlockSize(fileSize);

    // Handle empty files
    if (fileSize == 0) {
        blocks.emplace_back(0, 0, std::string());
        config.totalBlocks = 1;
        return;
    }

    // Calculate total number of blocks safely
    config.totalBlocks =
        fileSize / config.blockSize +
        (fileSize % config.blockSize != 0);

    // Reserve memory for expected blocks
    blocks.reserve(config.totalBlocks);

    size_t blockIndex = 0;

    // Partition file data
    for (size_t start = 0; start < fileSize; ) {

        const size_t currentBlockSize =
            (fileSize - start < config.blockSize)
            ? fileSize - start
            : config.blockSize;

        std::string blockContent =
            fileData.substr(start, currentBlockSize);

        blocks.emplace_back(
            static_cast<int>(blockIndex),
            start,
            blockContent
        );

        start += currentBlockSize;
        ++blockIndex;
    }
}
