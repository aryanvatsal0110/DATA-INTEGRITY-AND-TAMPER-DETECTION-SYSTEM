
#ifndef PARTITIONER_H
#define PARTITIONER_H

#include <string>
#include <vector>

#include "../tree/Block.h"
#include "../profiling/DataProfiler.h"
std::vector<Block> adaptivePartition(
    const std::vector<std::string>& records,
    const DataProfile& profile
);

void displayBlocks(const std::vector<Block>& blocks);

#endif
