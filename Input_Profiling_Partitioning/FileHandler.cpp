
#include "FileHandler.h"

#include <fstream>
#include <iterator>

bool FileHandler::readFile(const std::string& filePath,
                           std::string& fileData) const {

    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    fileData.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );

    if (file.bad()) {
        return false;
    }

    return true;
}
