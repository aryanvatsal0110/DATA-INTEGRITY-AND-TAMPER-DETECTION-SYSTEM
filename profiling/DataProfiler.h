#ifndef DATA_PROFILER_H
#define DATA_PROFILER_H

#include <string>
#include <vector>

struct DataProfile
{
    std::string fileName;
    std::string fileType;

    long long fileSize;

    int recordCount;

    double averageRecordSize;
};

DataProfile profileDataset(
    const std::string& filename,
    const std::vector<std::string>& records
);

void displayProfile(const DataProfile& profile);

#endif