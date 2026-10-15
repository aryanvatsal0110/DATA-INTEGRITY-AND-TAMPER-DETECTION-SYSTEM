
#ifndef DATASET_PIPELINE_H
#define DATASET_PIPELINE_H

#include <string>
#include <vector>

#include "../profiling/DataProfiler.h"
#include "../tree/Block.h"

struct DatasetPipelineResult
{
    std::vector<std::string> records;
    DataProfile profile;
    std::vector<Block> blocks;
};

DatasetPipelineResult prepareDataset(
    const std::string& filename
);

#endif
