
#include "ReportGenerator.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cstdint>
#include <system_error>

namespace fs = std::filesystem;

// --------------------------------------------------
// PART 1: Escape special characters for JSON
// --------------------------------------------------

std::string ReportGenerator::escapeJSON(
    const std::string& value
) const {

    std::ostringstream escaped;

    for (unsigned char ch : value) {

        switch (ch) {

            case '"':
                escaped << "\\\"";
                break;

            case '\\':
                escaped << "\\\\";
                break;

            case '\b':
                escaped << "\\b";
                break;

            case '\f':
                escaped << "\\f";
                break;

            case '\n':
                escaped << "\\n";
                break;

            case '\r':
                escaped << "\\r";
                break;

            case '\t':
                escaped << "\\t";
                break;

            default:

                if (ch < 0x20) {

                    escaped << "\\u"
                            << std::hex
                            << std::setw(4)
                            << std::setfill('0')
                            << static_cast<int>(ch)
                            << std::dec;

                }
                else {

                    escaped << static_cast<char>(ch);

                }
        }
    }

    return escaped.str();
}


// --------------------------------------------------
// PART 2: Extract file name from path
// --------------------------------------------------

std::string ReportGenerator::getFileName(
    const std::string& path
) const {

    return fs::path(path).filename().string();
}


// --------------------------------------------------
// PART 3: Generate timestamp and report ID
// --------------------------------------------------

std::string ReportGenerator::getTimestamp(
    std::string& reportId
) const {

    using namespace std::chrono;

    const auto now = system_clock::now();

    const auto millisecondsPart =
        duration_cast<milliseconds>(
            now.time_since_epoch()
        ).count() % 1000;

    const std::time_t currentTime =
        system_clock::to_time_t(now);

    std::tm localTime{};

#ifdef _WIN32
    localtime_s(&localTime, &currentTime);
#else
    localtime_r(&currentTime, &localTime);
#endif

    std::ostringstream timestamp;

    timestamp << std::put_time(
        &localTime,
        "%Y-%m-%d %H:%M:%S"
    );

    timestamp << "."
              << std::setw(3)
              << std::setfill('0')
              << millisecondsPart;

    const auto epochMilliseconds =
        duration_cast<milliseconds>(
            now.time_since_epoch()
        ).count();

    reportId = "TPD-" + std::to_string(epochMilliseconds);

    return timestamp.str();
}


// --------------------------------------------------
// PART 4: Convert block change enum to text
// --------------------------------------------------

std::string ReportGenerator::getChangeType(
    ChangeType type
) const {

    switch (type) {

        case ChangeType::MODIFIED:
            return "MODIFIED";

        case ChangeType::ADDED:
            return "ADDED";

        case ChangeType::REMOVED:
            return "REMOVED";
    }

    return "UNKNOWN";
}


// --------------------------------------------------
// PART 5: Convert line change enum to text
// --------------------------------------------------

std::string ReportGenerator::getLineChangeType(
    LineChangeType type
) const {

    switch (type) {

        case LineChangeType::MODIFIED:
            return "MODIFIED";

        case LineChangeType::ADDED:
            return "ADDED";

        case LineChangeType::REMOVED:
            return "REMOVED";
    }

    return "UNKNOWN";
}


// --------------------------------------------------
// PART 6: Generate complete JSON report
// --------------------------------------------------

bool ReportGenerator::generateJSONReport(
    const ReportInput& input,
    ReportResult& result
) const {

    result = ReportResult();

    // Validate required paths
    if (input.originalFilePath.empty() ||
        input.currentFilePath.empty() ||
        input.reportDirectory.empty()) {

        result.message = "Invalid report input paths.";

        return false;
    }

    // Validate consistency of supplied results
    if (input.verification.integrityMatch &&
        !input.localization.changes.empty()) {

        result.message =
            "Inconsistent input: verified file contains localized changes.";

        return false;
    }

    if (!input.localization.success) {

        result.message =
            "Block localization results are not available.";

        return false;
    }

    // Create report directory
    std::error_code error;

    fs::create_directories(
        input.reportDirectory,
        error
    );

    if (error) {

        result.message =
            "Unable to create report directory: " +
            error.message();

        return false;
    }

    // Generate report identity
    result.generatedAt = getTimestamp(result.reportId);

    const fs::path finalPath =
        fs::path(input.reportDirectory) /
        (result.reportId + ".json");

    const fs::path temporaryPath =
        finalPath.string() + ".tmp";

    result.reportPath = finalPath.string();

    std::ofstream output(
        temporaryPath,
        std::ios::binary | std::ios::trunc
    );

    if (!output.is_open()) {

        result.message = "Unable to create temporary report file.";

        result.reportPath.clear();

        return false;
    }

    // --------------------------------------------------
    // Write JSON
    // --------------------------------------------------

    output << "{\n";

    output << "  \"schema_version\": \"1.0\",\n";

    output << "  \"report_id\": \""
           << escapeJSON(result.reportId) << "\",\n";

    output << "  \"generated_at\": \""
           << escapeJSON(result.generatedAt) << "\",\n";


    // File information
    output << "  \"files\": {\n";

    output << "    \"original\": {\n";

    output << "      \"name\": \""
           << escapeJSON(getFileName(input.originalFilePath))
           << "\",\n";

    output << "      \"size_bytes\": "
           << input.metadata.originalFileSize << "\n";

    output << "    },\n";

    output << "    \"current\": {\n";

    output << "      \"name\": \""
           << escapeJSON(getFileName(input.currentFilePath))
           << "\",\n";

    output << "      \"size_bytes\": "
           << input.currentFileSize << "\n";

    output << "    }\n";

    output << "  },\n";


    // Integrity information
    output << "  \"integrity\": {\n";

    output << "    \"status\": \""
           << (input.verification.integrityMatch
                   ? "VERIFIED"
                   : "TAMPERED")
           << "\",\n";

    output << "    \"original_merkle_root\": \""
           << escapeJSON(input.verification.originalRoot)
           << "\",\n";

    output << "    \"current_merkle_root\": \""
           << escapeJSON(input.verification.currentRoot)
           << "\",\n";

    output << "    \"block_size\": "
           << input.metadata.blockSize << ",\n";

    output << "    \"original_block_count\": "
           << input.verification.originalBlockCount << ",\n";

    output << "    \"current_block_count\": "
           << input.verification.currentBlockCount << "\n";

    output << "  },\n";


    // Block-level changes
    size_t modifiedCount = 0;
    size_t addedCount = 0;
    size_t removedCount = 0;

    for (const BlockChange& change : input.localization.changes) {

        switch (change.changeType) {

            case ChangeType::MODIFIED:
                ++modifiedCount;
                break;

            case ChangeType::ADDED:
                ++addedCount;
                break;

            case ChangeType::REMOVED:
                ++removedCount;
                break;
        }
    }

    output << "  \"block_changes\": {\n";

    output << "    \"modified_count\": "
           << modifiedCount << ",\n";

    output << "    \"added_count\": "
           << addedCount << ",\n";

    output << "    \"removed_count\": "
           << removedCount << ",\n";

    output << "    \"details\": [\n";

    for (size_t i = 0; i < input.localization.changes.size(); ++i) {

        const BlockChange& change =
            input.localization.changes[i];

        output << "      {\n";

        output << "        \"block_index\": "
               << change.blockIndex << ",\n";

        output << "        \"change_type\": \""
               << getChangeType(change.changeType) << "\",\n";

        output << "        \"start_byte\": "
               << change.startByte << ",\n";

        output << "        \"original_size\": "
               << change.originalSize << ",\n";

        output << "        \"current_size\": "
               << change.currentSize << "\n";

        output << "      }";

        if (i + 1 < input.localization.changes.size()) {
            output << ",";
        }

        output << "\n";
    }

    output << "    ]\n";

    output << "  },\n";


    // Text-level changes
    output << "  \"text_comparison\": {\n";

    output << "    \"available\": "
           << (input.comparison.success ? "true" : "false")
           << ",\n";

    output << "    \"original_line_count\": "
           << input.comparison.originalLineCount << ",\n";

    output << "    \"current_line_count\": "
           << input.comparison.currentLineCount << ",\n";

    output << "    \"changes\": [\n";

    if (input.comparison.success) {

        for (size_t i = 0; i < input.comparison.changes.size(); ++i) {

            const LineChange& change =
                input.comparison.changes[i];

            output << "      {\n";

            output << "        \"change_type\": \""
                   << getLineChangeType(change.changeType)
                   << "\",\n";

            output << "        \"original_line\": "
                   << change.originalLineNumber << ",\n";

            output << "        \"current_line\": "
                   << change.currentLineNumber << ",\n";

            output << "        \"original_text\": \""
                   << escapeJSON(change.originalText) << "\",\n";

            output << "        \"current_text\": \""
                   << escapeJSON(change.currentText) << "\",\n";


            // Removed words
            output << "        \"removed_words\": [";

            for (size_t j = 0; j < change.removedWords.size(); ++j) {

                output << "\""
                       << escapeJSON(change.removedWords[j])
                       << "\"";

                if (j + 1 < change.removedWords.size()) {
                    output << ", ";
                }
            }

            output << "],\n";


            // Added words
            output << "        \"added_words\": [";

            for (size_t j = 0; j < change.addedWords.size(); ++j) {

                output << "\""
                       << escapeJSON(change.addedWords[j])
                       << "\"";

                if (j + 1 < change.addedWords.size()) {
                    output << ", ";
                }
            }

            output << "]\n";

            output << "      }";

            if (i + 1 < input.comparison.changes.size()) {
                output << ",";
            }

            output << "\n";
        }
    }

    output << "    ]\n";

    output << "  }\n";

    output << "}\n";

    output.close();

    if (!output) {

        std::error_code removeError;
        fs::remove(temporaryPath, removeError);

        result.message = "Failed while writing JSON report.";

        result.reportPath.clear();

        return false;
    }

    // Move completed temporary report to its final location
    fs::rename(
        temporaryPath,
        finalPath,
        error
    );

    if (error) {

        std::error_code removeError;
        fs::remove(temporaryPath, removeError);

        result.message =
            "Unable to finalize JSON report: " +
            error.message();

        result.reportPath.clear();

        return false;
    }

    result.success = true;

    result.message = "JSON report generated successfully.";

    return true;
}
