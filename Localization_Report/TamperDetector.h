
#ifndef TAMPER_DETECTOR_H
#define TAMPER_DETECTOR_H

#include "IntegrityVerifier.h"
#include "RootManager.h"

#include <vector>
#include <string>
#include <cstddef>

enum class ChangeType {
    MODIFIED,
    ADDED,
    REMOVED
};

struct BlockChange {
    size_t blockIndex = 0;

    ChangeType changeType = ChangeType::MODIFIED;

    size_t startByte = 0;

    size_t originalSize = 0;

    size_t currentSize = 0;
};

struct LocalizationResult {
    bool success = false;

    std::vector<BlockChange> changes;

    std::string message;
};

class TamperDetector {
public:

    bool detectChanges(
        const VerificationResult& verification,
        const RootMetadata& metadata,
        size_t currentFileSize,
        LocalizationResult& result
    ) const;

private:

    size_t calculateBlockSize(
        size_t fileSize,
        size_t startByte,
        size_t blockSize
    ) const;

};

#endif
