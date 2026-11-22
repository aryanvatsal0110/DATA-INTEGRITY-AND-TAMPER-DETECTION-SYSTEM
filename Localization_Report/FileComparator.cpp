
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

// --------------------------------------------------
// PART 7: Parse CSV content safely
// --------------------------------------------------

bool FileComparator::parseCSV(
    const std::string& content,
    std::vector<std::vector<std::string>>& rows,
    std::string& error
) const {

    rows.clear();
    error.clear();

    if (content.empty()) {
        return true;
    }

    std::vector<std::string> row;
    std::string field;

    bool insideQuotes = false;
    bool fieldStarted = false;

    for (size_t i = 0; i < content.size(); ++i) {

        char ch = content[i];

        if (insideQuotes) {

            if (ch == '"') {

                // Escaped quote inside a quoted CSV field: ""
                if (i + 1 < content.size() && content[i + 1] == '"') {
                    field.push_back('"');
                    ++i;
                }
                else {
                    insideQuotes = false;
                }

            }
            else {
                // Commas and newlines inside quotes are field content.
                field.push_back(ch);
            }

            continue;
        }

        if (ch == '"') {

            if (fieldStarted || !field.empty()) {
                error = "Invalid CSV: quote found inside an unquoted field.";
                return false;
            }

            insideQuotes = true;
            fieldStarted = true;
        }
        else if (ch == ',') {

            row.push_back(field);
            field.clear();
            fieldStarted = false;
        }
        else if (ch == '\n') {

            if (!field.empty() && field.back() == '\r') {
                field.pop_back();
            }

            row.push_back(field);
            rows.push_back(row);

            row.clear();
            field.clear();
            fieldStarted = false;
        }
        else {

            if (ch != '\r') {
                field.push_back(ch);
                fieldStarted = true;
            }
        }
    }

    if (insideQuotes) {
        error = "Invalid CSV: quoted field was not closed.";
        return false;
    }

    // Do not create an extra row for a final newline.
    if (!row.empty() || !field.empty() || fieldStarted) {
        row.push_back(field);
        rows.push_back(row);
    }

    return true;
}


// --------------------------------------------------
// PART 8: Compare CSV rows and individual cells
// --------------------------------------------------

bool FileComparator::compareCSV(
    const std::string& original,
    const std::string& current,
    FileComparisonResult& result
) const {

    result = FileComparisonResult();

    std::vector<std::vector<std::string>> oldRows;
    std::vector<std::vector<std::string>> newRows;

    std::string parseError;

    if (!parseCSV(original, oldRows, parseError)) {
        result.message = "Original CSV parsing failed: " + parseError;
        return false;
    }

    if (!parseCSV(current, newRows, parseError)) {
        result.message = "Current CSV parsing failed: " + parseError;
        return false;
    }

    result.originalLineCount = oldRows.size();
    result.currentLineCount = newRows.size();

    // Convert rows into stable comparison strings so LCS can align
    // unchanged rows even when rows have been inserted or removed.
    auto serializeRow = [](const std::vector<std::string>& cells) {

        std::string serialized;

        for (size_t i = 0; i < cells.size(); ++i) {

            if (i > 0) {
                serialized.push_back('\x1F');
            }

            serialized += cells[i];
        }

        return serialized;
    };

    std::vector<std::string> oldSerialized;
    std::vector<std::string> newSerialized;

    oldSerialized.reserve(oldRows.size());
    newSerialized.reserve(newRows.size());

    for (const auto& row : oldRows) {
        oldSerialized.push_back(serializeRow(row));
    }

    for (const auto& row : newRows) {
        newSerialized.push_back(serializeRow(row));
    }

    std::vector<std::pair<size_t, size_t>> matches;

    findLCS(
        oldSerialized,
        0,
        oldSerialized.size(),
        newSerialized,
        0,
        newSerialized.size(),
        matches
    );

    // The first CSV row is treated as the header when available.
    auto columnName = [&](size_t column) -> std::string {

        if (!oldRows.empty() &&
            column < oldRows.front().size() &&
            !oldRows.front()[column].empty()) {
            return oldRows.front()[column];
        }

        if (!newRows.empty() &&
            column < newRows.front().size() &&
            !newRows.front()[column].empty()) {
            return newRows.front()[column];
        }

        return "Column " + std::to_string(column + 1);
    };

    auto compareMatchedRows = [&](size_t oldIndex, size_t newIndex) {

        const auto& oldRow = oldRows[oldIndex];
        const auto& newRow = newRows[newIndex];

        const size_t maxColumns =
            std::max(oldRow.size(), newRow.size());

        for (size_t column = 0; column < maxColumns; ++column) {

            const bool existsOld = column < oldRow.size();
            const bool existsNew = column < newRow.size();

            const std::string oldValue =
                existsOld ? oldRow[column] : "";

            const std::string newValue =
                existsNew ? newRow[column] : "";

            if (existsOld && existsNew && oldValue == newValue) {
                continue;
            }

            LineChange change;

            change.originalLineNumber = oldIndex + 1;
            change.currentLineNumber = newIndex + 1;

            const std::string name = columnName(column);

            if (existsOld && existsNew) {

                change.changeType = LineChangeType::MODIFIED;

                change.originalText =
                    name + ": " + oldValue;

                change.currentText =
                    name + ": " + newValue;
            }
            else if (existsOld) {

                change.changeType = LineChangeType::REMOVED;

                change.originalText =
                    name + ": " + oldValue;
            }
            else {

                change.changeType = LineChangeType::ADDED;

                change.currentText =
                    name + ": " + newValue;
            }

            result.changes.push_back(std::move(change));
        }
    };

    size_t oldIndex = 0;
    size_t newIndex = 0;

    auto processGap = [&](size_t oldEnd, size_t newEnd) {

        const size_t removedCount = oldEnd - oldIndex;
        const size_t addedCount = newEnd - newIndex;

        const size_t pairedCount =
            std::min(removedCount, addedCount);

        // Pair changed rows first, then compare their individual cells.
        for (size_t i = 0; i < pairedCount; ++i) {

            compareMatchedRows(oldIndex, newIndex);

            ++oldIndex;
            ++newIndex;
        }

        // Remaining original rows were removed.
        while (oldIndex < oldEnd) {

            LineChange change;

            change.changeType = LineChangeType::REMOVED;
            change.originalLineNumber = oldIndex + 1;

            for (size_t column = 0; column < oldRows[oldIndex].size(); ++column) {

                if (column > 0) {
                    change.originalText += " | ";
                }

                change.originalText +=
                    columnName(column) + ": " +
                    oldRows[oldIndex][column];
            }

            result.changes.push_back(std::move(change));
            ++oldIndex;
        }

        // Remaining current rows were added.
        while (newIndex < newEnd) {

            LineChange change;

            change.changeType = LineChangeType::ADDED;
            change.currentLineNumber = newIndex + 1;

            for (size_t column = 0; column < newRows[newIndex].size(); ++column) {

                if (column > 0) {
                    change.currentText += " | ";
                }

                change.currentText +=
                    columnName(column) + ": " +
                    newRows[newIndex][column];
            }

            result.changes.push_back(std::move(change));
            ++newIndex;
        }
    };

    for (const auto& match : matches) {

        processGap(match.first, match.second);

        oldIndex = match.first + 1;
        newIndex = match.second + 1;
    }

    processGap(oldRows.size(), newRows.size());

    result.success = true;

    result.message = result.changes.empty()
        ? "No CSV differences detected."
        : "CSV comparison completed. Row and cell differences localized.";

    return true;
}


// --------------------------------------------------
// PART 9: Compare two files
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

    auto getExtension = [](const std::string& path) {

        const size_t slash = path.find_last_of("/\\\\");
        const size_t dot = path.find_last_of('.');

        if (dot == std::string::npos ||
            (slash != std::string::npos && dot < slash)) {
            return std::string();
        }

        std::string extension = path.substr(dot + 1);

        std::transform(
            extension.begin(),
            extension.end(),
            extension.begin(),
            [](unsigned char ch) {
                return static_cast<char>(std::tolower(ch));
            }
        );

        return extension;
    };

    const std::string originalExtension = getExtension(originalPath);
    const std::string currentExtension = getExtension(currentPath);

    if (originalExtension != currentExtension) {

        result.message =
            "File types do not match. Please verify using the same file format.";

        return false;
    }

    if (originalExtension == "csv") {
        return compareCSV(originalData, currentData, result);
    }

    return compareText(
        originalData,
        currentData,
        result
    );
}
