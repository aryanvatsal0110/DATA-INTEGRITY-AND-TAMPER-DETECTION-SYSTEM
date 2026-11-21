
#include "FileComparator.h"

#include <algorithm>
#include <cctype>
#include <utility>
#include <stdexcept>

// --------------------------------------------------
// PART 1: Split content into lines
// --------------------------------------------------

std::vector<std::string> FileComparator::splitLines(
    const std::string& content
) const {

    std::vector<std::string> lines;

    if (content.empty()) {
        return lines;
    }

    size_t start = 0;

    while (start < content.size()) {

        size_t end = content.find('\n', start);

        if (end == std::string::npos) {

            lines.push_back(content.substr(start));

            break;
        }

        std::string line =
            content.substr(start, end - start);

        // Remove CR from Windows CRLF line endings
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        lines.push_back(line);

        start = end + 1;
    }

    return lines;
}


// --------------------------------------------------
// PART 2: Split a line into words and punctuation
// --------------------------------------------------

std::vector<std::string> FileComparator::splitWords(
    const std::string& line
) const {

    std::vector<std::string> words;

    size_t i = 0;

    while (i < line.size()) {

        unsigned char ch =
            static_cast<unsigned char>(line[i]);

        if (std::isspace(ch)) {
            ++i;
            continue;
        }

        // Keep alphanumeric sequences and underscores together
        if (std::isalnum(ch) || ch == '_') {

            size_t start = i;

            while (i < line.size()) {

                unsigned char current =
                    static_cast<unsigned char>(line[i]);

                if (!std::isalnum(current) && current != '_') {
                    break;
                }

                ++i;
            }

            words.push_back(
                line.substr(start, i - start)
            );
        }
        else {

            // Treat punctuation as individual tokens
            words.push_back(
                line.substr(i, 1)
            );

            ++i;
        }
    }

    return words;
}


// --------------------------------------------------
// PART 3: Calculate one LCS dynamic programming row
// Uses O(min(n,m)) auxiliary memory
// --------------------------------------------------

std::vector<size_t> FileComparator::calculateLCSRow(
    const std::vector<std::string>& a,
    size_t aStart,
    size_t aEnd,
    const std::vector<std::string>& b,
    size_t bStart,
    size_t bEnd,
    bool reverse
) const {

    const size_t n = aEnd - aStart;
    const size_t m = bEnd - bStart;

    std::vector<size_t> dp(m + 1, 0);

    for (size_t i = 0; i < n; ++i) {

        size_t diagonal = 0;

        for (size_t j = 1; j <= m; ++j) {

            size_t ai = reverse
                ? aEnd - 1 - i
                : aStart + i;

            size_t bj = reverse
                ? bEnd - j
                : bStart + j - 1;

            size_t previous = dp[j];

            if (a[ai] == b[bj]) {

                dp[j] = diagonal + 1;

            }
            else {

                dp[j] = std::max(dp[j], dp[j - 1]);

            }

            diagonal = previous;
        }
    }

    return dp;
}


// --------------------------------------------------
// PART 4: Hirschberg LCS reconstruction
// Stores matching index pairs
// --------------------------------------------------

void FileComparator::findLCS(
    const std::vector<std::string>& a,
    size_t aStart,
    size_t aEnd,
    const std::vector<std::string>& b,
    size_t bStart,
    size_t bEnd,
    std::vector<std::pair<size_t, size_t>>& matches
) const {

    if (aStart >= aEnd || bStart >= bEnd) {
        return;
    }

    // Base case: one element in A
    if (aEnd - aStart == 1) {

        for (size_t j = bStart; j < bEnd; ++j) {

            if (a[aStart] == b[j]) {

                matches.emplace_back(aStart, j);

                break;
            }
        }

        return;
    }

    const size_t middle =
        aStart + (aEnd - aStart) / 2;

    std::vector<size_t> left =
        calculateLCSRow(
            a,
            aStart,
            middle,
            b,
            bStart,
            bEnd,
            false
        );

    std::vector<size_t> right =
        calculateLCSRow(
            a,
            middle,
            aEnd,
            b,
            bStart,
            bEnd,
            true
        );

    const size_t m = bEnd - bStart;

    size_t bestSplit = 0;
    size_t bestLength = 0;

    for (size_t j = 0; j <= m; ++j) {

        size_t candidate =
            left[j] + right[m - j];

        if (candidate > bestLength) {

            bestLength = candidate;

            bestSplit = j;
        }
    }

    const size_t bMiddle =
        bStart + bestSplit;

    // Release temporary rows before recursion
    left.clear();
    right.clear();

    left.shrink_to_fit();
    right.shrink_to_fit();

    findLCS(
        a,
        aStart,
        middle,
        b,
        bStart,
        bMiddle,
        matches
    );

    findLCS(
        a,
        middle,
        aEnd,
        b,
        bMiddle,
        bEnd,
        matches
    );
}


// --------------------------------------------------
// PART 5: Compare words in a modified line
// --------------------------------------------------

void FileComparator::compareWords(
    const std::string& original,
    const std::string& current,
    std::vector<std::string>& removed,
    std::vector<std::string>& added
) const {

    std::vector<std::string> oldWords =
        splitWords(original);

    std::vector<std::string> newWords =
        splitWords(current);

    std::vector<std::pair<size_t, size_t>> matches;

    findLCS(
        oldWords,
        0,
        oldWords.size(),
        newWords,
        0,
        newWords.size(),
        matches
    );

    size_t oldIndex = 0;
    size_t newIndex = 0;

    for (const auto& match : matches) {

        size_t oldMatch = match.first;
        size_t newMatch = match.second;

        while (oldIndex < oldMatch) {
            removed.push_back(oldWords[oldIndex++]);
        }

        while (newIndex < newMatch) {
            added.push_back(newWords[newIndex++]);
        }

        oldIndex = oldMatch + 1;
        newIndex = newMatch + 1;
    }

    while (oldIndex < oldWords.size()) {
        removed.push_back(oldWords[oldIndex++]);
    }

    while (newIndex < newWords.size()) {
        added.push_back(newWords[newIndex++]);
    }
}


// --------------------------------------------------
// PART 6: Compare two text contents
// --------------------------------------------------

bool FileComparator::compareText(
    const std::string& original,
    const std::string& current,
    FileComparisonResult& result
) const {

    result = FileComparisonResult();

    std::vector<std::string> oldLines =
        splitLines(original);

    std::vector<std::string> newLines =
        splitLines(current);

    result.originalLineCount = oldLines.size();
    result.currentLineCount = newLines.size();

    std::vector<std::pair<size_t, size_t>> matches;

    findLCS(
        oldLines,
        0,
        oldLines.size(),
        newLines,
        0,
        newLines.size(),
        matches
    );

    size_t oldIndex = 0;
    size_t newIndex = 0;

    auto processGap = [&](size_t oldEnd, size_t newEnd) {

        size_t removedCount = oldEnd - oldIndex;
        size_t addedCount = newEnd - newIndex;

        size_t pairedCount =
            std::min(removedCount, addedCount);

        // Pair removed and added lines as modifications
        for (size_t i = 0; i < pairedCount; ++i) {

            LineChange change;

            change.changeType = LineChangeType::MODIFIED;

            change.originalLineNumber = oldIndex + 1;
            change.currentLineNumber = newIndex + 1;

            change.originalText = oldLines[oldIndex];
            change.currentText = newLines[newIndex];

            compareWords(
                change.originalText,
                change.currentText,
                change.removedWords,
                change.addedWords
            );

            result.changes.push_back(change);

            ++oldIndex;
            ++newIndex;
        }

        // Remaining old lines are removals
        while (oldIndex < oldEnd) {

            LineChange change;

            change.changeType = LineChangeType::REMOVED;

            change.originalLineNumber = oldIndex + 1;

            change.originalText = oldLines[oldIndex];

            result.changes.push_back(change);

            ++oldIndex;
        }

        // Remaining new lines are additions
        while (newIndex < newEnd) {

            LineChange change;

            change.changeType = LineChangeType::ADDED;

            change.currentLineNumber = newIndex + 1;

            change.currentText = newLines[newIndex];

            result.changes.push_back(change);

            ++newIndex;
        }
    };

    // Process gaps around all matching lines
    for (const auto& match : matches) {

        processGap(match.first, match.second);

        // Skip the matching line
        oldIndex = match.first + 1;
        newIndex = match.second + 1;
    }

    // Process the remaining lines
    processGap(oldLines.size(), newLines.size());

    result.success = true;

    if (result.changes.empty()) {

        result.message = "No text differences detected.";

    }
    else {

        result.message =
            "Text comparison completed. Differences localized.";

    }

    return true;
}


// --------------------------------------------------
// PART 7: Compare two files
// --------------------------------------------------

bool FileComparator::compareFiles(
    const std::string& originalPath,
    const std::string& currentPath,
    FileComparisonResult& result
) const {

    result = FileComparisonResult();

    if (originalPath.empty() || currentPath.empty()) {

        result.message = "Invalid file paths.";

        return false;
    }

    std::string originalData;
    std::string currentData;

    if (!fileHandler.readFile(originalPath, originalData)) {

        result.message = "Failed to read original file.";

        return false;
    }

    if (!fileHandler.readFile(currentPath, currentData)) {

        result.message = "Failed to read current file.";

        return false;
    }

    return compareText(
        originalData,
        currentData,
        result
    );
}
