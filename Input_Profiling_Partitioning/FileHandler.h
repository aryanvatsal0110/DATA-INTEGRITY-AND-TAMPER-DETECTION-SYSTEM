
#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>

class FileHandler {
public:
    bool readFile(const std::string& filePath,
                  std::string& fileData) const;
};

#endif
