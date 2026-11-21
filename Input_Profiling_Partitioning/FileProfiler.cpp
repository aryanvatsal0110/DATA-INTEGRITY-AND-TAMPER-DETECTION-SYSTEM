
#include "FileProfiler.h"

#include <filesystem>
#include <algorithm>
#include <cctype>
#include <system_error>

namespace fs = std::filesystem;

bool FileProfiler::profileFile(const std::string& filePath,
                               FileProfile& profile) const {

    std::error_code error;

    fs::path path(filePath);

    if (!fs::exists(path, error) || error) {
        return false;
    }

    if (!fs::is_regular_file(path, error) || error) {
        return false;
    }

    profile.fileName = path.filename().string();

    profile.extension = path.extension().string();

    std::transform(
        profile.extension.begin(),
        profile.extension.end(),
        profile.extension.begin(),
        [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        }
    );

    profile.fileSize = fs::file_size(path, error);

    if (error) {
        return false;
    }

    profile.fileType = identifyFileType(profile.extension);

    return true;
}

std::string FileProfiler::identifyFileType(
    const std::string& extension) const {

    if (extension == ".txt")
        return "Text";

    if (extension == ".csv")
        return "CSV";

    if (extension == ".pdf")
        return "PDF";

    if (extension == ".docx")
        return "Word Document";

    if (extension == ".jpg" || extension == ".jpeg" ||
        extension == ".png")
        return "Image";

    return "Other";
}
