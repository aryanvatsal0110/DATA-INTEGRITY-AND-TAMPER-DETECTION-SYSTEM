
#include "DatasetPipeline.h"

#include "InputHandler.h"
#include "../partitioning/Partitioner.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

DatasetPipelineResult prepareDataset(
    const std::string& filename)
{
    DatasetPipelineResult result;

    // 1. Read the real dataset.
    result.records = readDataset(filename);

    if (result.records.empty())
    {
        throw std::runtime_error(
            "Dataset is empty or could not be read: " + filename
        );
    }

    // 2. Profile the dataset.
    result.profile = profileDataset(
        filename,
        result.records
    );

    // 3. Partition records into Block objects.
    result.blocks = adaptivePartition(
        result.records,
        result.profile
    );

    // 4. Display the processing summary.
    displayProfile(result.profile);
    displayBlocks(result.blocks);

    return result;
}
