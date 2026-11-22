#ifndef FILE_COMPARATOR_H
#define FILE_COMPARATOR_H

#include "FileHandler.h"

#include <string>
#include <vector>
#include <cstddef>
#include <utility>

enum class LineChangeType {
    MODIFIED,
    ADDED,
    REMOVED
};

struct LineChange {
    LineChangeType changeType;

    size_t originalLineNumber = 0;
    size_t currentLineNumber = 0;

    std::string originalText;
    std::string currentText;

    std::vector<std::string> removedWords;
    std::vector<std::string> addedWords;
};

struct FileComparisonResult {
    bool success = false;

    size_t originalLineCount = 0;
    size_t currentLineCount = 0;

    std::vector<LineChange> changes;

    std::string message;
};

class FileComparator {
private:

    FileHandler fileHandler;

    std::vector<std::string> splitLines(
        const std::string& content
    ) const;

    std::vector<std::string> splitWords(
        const std::string& line
    ) const;

    void findLCS(
        const std::vector<std::string>& a,
        size_t aStart,
        size_t aEnd,
        const std::vector<std::string>& b,
        size_t bStart,
        size_t bEnd,
        std::vector<std::pair<size_t, size_t>>& matches
    ) const;

    std::vector<size_t> calculateLCSRow(
        const std::vector<std::string>& a,
        size_t aStart,
        size_t aEnd,
        const std::vector<std::string>& b,
        size_t bStart,
        size_t bEnd,
        bool reverse
    ) const;

    void compareWords(
        const std::string& original,
        const std::string& current,
        std::vector<std::string>& removed,
        std::vector<std::string>& added
    ) const;

    // CSV parser supports quoted fields, escaped quotes,
    // commas inside quoted fields, and embedded newlines.
    bool parseCSV(
        const std::string& content,
        std::vector<std::vector<std::string>>& rows,
        std::string& error
    ) const;

    bool compareCSV(
        const std::string& original,
        const std::string& current,
        FileComparisonResult& result
    ) const;

public:

    bool compareText(
        const std::string& original,
        const std::string& current,
        FileComparisonResult& result
    ) const;

    bool compareFiles(
        const std::string& originalPath,
        const std::string& currentPath,
        FileComparisonResult& result
    ) const;
};

#endif
