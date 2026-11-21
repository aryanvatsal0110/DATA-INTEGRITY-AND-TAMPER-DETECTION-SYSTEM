
#ifndef FILE_PROFILER_H
#define FILE_PROFILER_H

#include <string>
#include <cstddef>

struct FileProfile {
    std::string fileName;
    std::string extension;
    std::string fileType;
    size_t fileSize;
};

class FileProfiler {
public:
    bool profileFile(const std::string& filePath,
                     FileProfile& profile) const;

private:
    std::string identifyFileType(const std::string& extension) const;
};

#endif
