
#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include "IntegrityVerifier.h"
#include "TamperDetector.h"
#include "FileComparator.h"

#include <string>

struct ReportInput {
    std::string originalFilePath;
    std::string currentFilePath;
    std::string reportDirectory;

    size_t currentFileSize = 0;

    VerificationResult verification;
    RootMetadata metadata;

    LocalizationResult localization;
    FileComparisonResult comparison;
};

struct ReportResult {
    bool success = false;

    std::string reportId;
    std::string generatedAt;
    std::string reportPath;

    std::string message;
};

class ReportGenerator {
public:

    bool generateJSONReport(
        const ReportInput& input,
        ReportResult& result
    ) const;

private:

    std::string escapeJSON(
        const std::string& value
    ) const;

    std::string getFileName(
        const std::string& path
    ) const;

    std::string getTimestamp(
        std::string& reportId
    ) const;

    std::string getChangeType(
        ChangeType type
    ) const;

    std::string getLineChangeType(
        LineChangeType type
    ) const;
};

#endif
