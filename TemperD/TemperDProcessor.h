
#ifndef TEMPERD_PROCESSOR_H
#define TEMPERD_PROCESSOR_H

#include "FileProfiler.h"
#include "FileHandler.h"
#include "Partitioner.h"
#include "HashGenerator.h"
#include "MerkleTree.h"
#include "RootManager.h"
#include "BaselineStorage.h"
#include "IntegrityVerifier.h"
#include "TamperDetector.h"
#include "FileComparator.h"
#include "ReportGenerator.h"

#include <string>
#include <cstddef>

struct RegistrationResult {
    bool success = false;

    std::string fileId;
    std::string originalFilePath;
    std::string baselinePath;
    std::string merkleRoot;

    size_t fileSize = 0;
    size_t totalBlocks = 0;
    size_t blockSize = 0;

    std::string message;
};

struct TemperDVerificationResult {
    bool success = false;

    VerificationResult verification;
    LocalizationResult localization;
    FileComparisonResult comparison;
    ReportResult report;

    std::string message;
};

class TemperDProcessor {

private:

    FileProfiler profiler;
    FileHandler fileHandler;
    Partitioner partitioner;
    HashGenerator hashGenerator;
    RootManager rootManager;
    BaselineStorage baselineStorage;
    IntegrityVerifier integrityVerifier;
    TamperDetector tamperDetector;
    FileComparator fileComparator;
    ReportGenerator reportGenerator;

    std::string createFileId() const;

    bool buildMerkleRoot(
        const std::vector<std::string>& hashes,
        std::string& root
    ) const;

public:

    bool registerFile(
        const std::string& uploadedFilePath,
        const std::string& storageRoot,
        RegistrationResult& result
    ) const;

    bool verifyRegisteredFile(
        const std::string& uploadedFilePath,
        const std::string& fileId,
        const std::string& storageRoot,
        TemperDVerificationResult& result
    ) const;
};

#endif
