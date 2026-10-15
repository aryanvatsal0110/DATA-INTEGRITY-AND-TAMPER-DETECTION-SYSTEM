
#ifndef PARTITIONER_H
#define PARTITIONER_H

#include <string>
#include <vector>

#include "../tree/Block.h"
#include "../profiling/DataProfiler.h"

// Divide dataset records into blocks using the data profile.
std::vector<Block> adaptivePartition(
    const std::vector<std::string>& records,
    const DataProfile& profile
);

void displayBlocks(const std::vector<Block>& blocks);

#endif
