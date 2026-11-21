
#include "TamperDetector.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

// --------------------------------------------------
// Calculate actual size of a block
// --------------------------------------------------

size_t TamperDetector::calculateBlockSize(
    size_t fileSize,
    size_t startByte,
    size_t blockSize
) const {

    if (startByte >= fileSize) {
        return 0;
    }

    return std::min(blockSize, fileSize - startByte);
}


// --------------------------------------------------
// Detect modified, added and removed blocks
// --------------------------------------------------

bool TamperDetector::detectChanges(
    const VerificationResult& verification,
    const RootMetadata& metadata,
    size_t currentFileSize,
    LocalizationResult& result
) const {

    result = LocalizationResult();

    // Validate metadata
    if (metadata.blockSize == 0) {

        result.message = "Invalid block size.";

        return false;
    }

    if (verification.originalBlockCount == 0 ||
        verification.currentBlockCount == 0) {

        result.message = "Invalid block count.";

        return false;
    }

    const size_t originalCount =
        verification.originalBlockCount;

    const size_t currentCount =
        verification.currentBlockCount;

    const size_t blockSize =
        metadata.blockSize;

    const size_t commonCount =
        std::min(originalCount, currentCount);

    // Validate byte-offset multiplication before calculating ranges
    for (size_t index : verification.changedBlockIndices) {

        if (index > std::numeric_limits<size_t>::max() / blockSize) {

            result.message = "Block byte offset overflow.";

            return false;
        }

        if (index >= std::max(originalCount, currentCount)) {

            result.message = "Invalid changed block index.";

            return false;
        }
    }

    // Process every changed block
    for (size_t index : verification.changedBlockIndices) {

        BlockChange change;

        change.blockIndex = index;

        change.startByte = index * blockSize;

        // A block present in both versions is modified
        if (index < commonCount) {

            change.changeType = ChangeType::MODIFIED;

        }

        // A block existing only in the current file is added
        else if (index >= originalCount) {

            change.changeType = ChangeType::ADDED;

        }

        // A block existing only in the original file is removed
        else {

            change.changeType = ChangeType::REMOVED;

        }

        change.originalSize = calculateBlockSize(
            metadata.originalFileSize,
            change.startByte,
            blockSize
        );

        change.currentSize = calculateBlockSize(
            currentFileSize,
            change.startByte,
            blockSize
        );

        result.changes.push_back(change);
    }

    result.success = true;

    if (result.changes.empty() && verification.integrityMatch) {

        result.message = "No tampering detected.";

    }
    else if (result.changes.empty()) {

        result.message =
            "Integrity mismatch detected, but no changed block indices were supplied.";

    }
    else {

        result.message =
            "Tampering detected and affected blocks localized.";

    }

    return true;
}
