
#ifndef BASELINE_STORAGE_H
#define BASELINE_STORAGE_H

#include "RootManager.h"

#include <string>
#include <vector>

struct BaselineData {
    RootMetadata metadata;
    std::vector<std::string> blockHashes;
};

class BaselineStorage {

public:

    bool saveBaseline(
        const std::string& filePath,
        const RootMetadata& metadata,
        const std::vector<std::string>& blockHashes
    ) const;

    bool loadBaseline(
        const std::string& filePath,
        BaselineData& baseline
    ) const;

private:

    bool validateBaseline(
        const RootMetadata& metadata,
        const std::vector<std::string>& blockHashes
    ) const;

};

#endif
