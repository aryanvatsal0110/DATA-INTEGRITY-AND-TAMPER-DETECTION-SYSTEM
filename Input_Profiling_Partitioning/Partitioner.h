
#ifndef PARTITIONER_H
#define PARTITIONER_H

#include "Block.h"

#include <string>
#include <vector>
#include <cstddef>

struct PartitionConfig {
    size_t blockSize = 0;
    size_t totalBlocks = 0;
};

class Partitioner {
public:
    void partitionFile(
        const std::string& fileData,
        std::vector<Block>& blocks,
        PartitionConfig& config
    ) const;

private:
    size_t selectBlockSize(size_t fileSize) const;
};

#endif
