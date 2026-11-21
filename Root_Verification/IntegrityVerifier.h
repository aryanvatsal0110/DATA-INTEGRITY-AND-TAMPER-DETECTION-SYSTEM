
#ifndef INTEGRITY_VERIFIER_H
#define INTEGRITY_VERIFIER_H

#include "BaselineStorage.h"
#include "FileHandler.h"
#include "HashGenerator.h"

#include <string>
#include <vector>
#include <cstddef>

struct VerificationResult {

    bool integrityMatch = false;

    size_t originalBlockCount = 0;

    size_t currentBlockCount = 0;

    std::string originalRoot;

    std::string currentRoot;

    std::vector<size_t> changedBlockIndices;

    std::string message;
};

class IntegrityVerifier {

private:

    FileHandler fileHandler;

    HashGenerator hashGenerator;

    BaselineStorage baselineStorage;

    bool partitionUsingBaselineConfig(
        const std::string& fileData,
        size_t blockSize,
        std::vector<Block>& blocks
    ) const;

    bool generateHashes(
        const std::vector<Block>& blocks,
        std::vector<std::string>& hashes
    ) const;

    bool generateMerkleRoot(
        const std::vector<std::string>& hashes,
        const RootMetadata& metadata,
        std::string& root
    ) const;

public:

    bool verifyFile(
        const std::string& currentFilePath,
        const std::string& baselinePath,
        VerificationResult& result
    ) const;
};

#endif
