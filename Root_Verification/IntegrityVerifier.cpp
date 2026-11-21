
#include "IntegrityVerifier.h"

#include "MerkleTree.h"
#include "HierarchicalMerkleTree.h"

#include <algorithm>
#include <limits>
#include <utility>


// --------------------------------------------------
// PART 1: Partition file using original configuration
// --------------------------------------------------

bool IntegrityVerifier::partitionUsingBaselineConfig(
    const std::string& fileData,
    size_t blockSize,
    std::vector<Block>& blocks
) const {

    blocks.clear();

    if (blockSize == 0) {
        return false;
    }

    size_t fileSize = fileData.size();

    // Empty files must still produce one empty block
    if (fileSize == 0) {

        blocks.emplace_back(0, 0, "");

        return true;
    }

    // Calculate required block count safely
    size_t totalBlocks =
        fileSize / blockSize +
        (fileSize % blockSize != 0);

    if (totalBlocks >
        static_cast<size_t>(std::numeric_limits<int>::max())) {

        return false;
    }

    blocks.reserve(totalBlocks);

    // Partition using the ORIGINAL block size
    for (size_t offset = 0;
         offset < fileSize;
         offset += blockSize) {

        size_t currentSize =
            std::min(blockSize, fileSize - offset);

        std::string blockData =
            fileData.substr(offset, currentSize);

        blocks.emplace_back(
            static_cast<int>(blocks.size()),
            offset,
            blockData
        );
    }

    return blocks.size() == totalBlocks;
}


// --------------------------------------------------
// PART 2: Generate hashes for all current blocks
// --------------------------------------------------

bool IntegrityVerifier::generateHashes(
    const std::vector<Block>& blocks,
    std::vector<std::string>& hashes
) const {

    return hashGenerator.generateBlockHashes(
        blocks,
        hashes
    );
}


// --------------------------------------------------
// PART 3: Reconstruct the correct Merkle Tree
// --------------------------------------------------

bool IntegrityVerifier::generateMerkleRoot(
    const std::vector<std::string>& hashes,
    const RootMetadata& metadata,
    std::string& root
) const {

    root.clear();

    if (hashes.empty()) {
        return false;
    }

    // Standard binary Merkle Tree
    if (metadata.treeType == TreeType::STANDARD) {

        MerkleTree tree;

        if (!tree.buildTree(hashes)) {
            return false;
        }

        root = tree.getRootHash();

        return root.size() == 64;
    }

    // Hierarchical Merkle Tree
    if (metadata.treeType == TreeType::HIERARCHICAL) {

        if (metadata.groupSize == 0) {
            return false;
        }

        HierarchicalMerkleTree tree;

        if (!tree.buildTree(
                hashes,
                metadata.groupSize
            )) {

            return false;
        }

        root = tree.getGlobalRootHash();

        return root.size() == 64;
    }

    return false;
}


// --------------------------------------------------
// PART 4: Complete file integrity verification
// --------------------------------------------------

bool IntegrityVerifier::verifyFile(
    const std::string& currentFilePath,
    const std::string& baselinePath,
    VerificationResult& result
) const {

    result = VerificationResult();

    // Step 1: Validate paths
    if (currentFilePath.empty() || baselinePath.empty()) {

        result.message = "Invalid file or baseline path.";

        return false;
    }


    // Step 2: Load original baseline
    BaselineData baseline;

    if (!baselineStorage.loadBaseline(
            baselinePath,
            baseline
        )) {

        result.message =
            "Failed to load or validate original baseline.";

        return false;
    }

    const RootMetadata& metadata = baseline.metadata;

    result.originalRoot = metadata.merkleRoot;

    result.originalBlockCount = baseline.blockHashes.size();


    // Step 3: Reconstruct the original Merkle Root
    // This also checks consistency between stored hashes and root.

    std::string reconstructedOriginalRoot;

    if (!generateMerkleRoot(
            baseline.blockHashes,
            metadata,
            reconstructedOriginalRoot
        )) {

        result.message =
            "Failed to reconstruct original Merkle Tree.";

        return false;
    }

    if (reconstructedOriginalRoot != metadata.merkleRoot) {

        result.message =
            "Baseline integrity check failed. "
            "Stored root does not match stored block hashes.";

        return false;
    }


    // Step 4: Read current file
    std::string currentFileData;

    if (!fileHandler.readFile(
            currentFilePath,
            currentFileData
        )) {

        result.message = "Unable to read current file.";

        return false;
    }


    // Step 5: Partition using original block size
    std::vector<Block> currentBlocks;

    if (!partitionUsingBaselineConfig(
            currentFileData,
            metadata.blockSize,
            currentBlocks
        )) {

        result.message =
            "Failed to partition current file.";

        return false;
    }

    result.currentBlockCount = currentBlocks.size();


    // Step 6: Generate current block hashes
    std::vector<std::string> currentHashes;

    if (!generateHashes(
            currentBlocks,
            currentHashes
        )) {

        result.message =
            "Failed to generate current block hashes.";

        return false;
    }


    // Step 7: Construct current Merkle Root
    if (!generateMerkleRoot(
            currentHashes,
            metadata,
            result.currentRoot
        )) {

        result.message =
            "Failed to construct current Merkle Tree.";

        return false;
    }


    // Step 8: Compare original and current block hashes
    const size_t originalCount =
        baseline.blockHashes.size();

    const size_t currentCount =
        currentHashes.size();

    const size_t commonCount =
        std::min(originalCount, currentCount);

    for (size_t i = 0; i < commonCount; i++) {

        if (baseline.blockHashes[i] != currentHashes[i]) {

            result.changedBlockIndices.push_back(i);
        }
    }

    // Record additional blocks if current file is larger
    for (size_t i = originalCount;
         i < currentCount;
         i++) {

        result.changedBlockIndices.push_back(i);
    }


    // Record removed blocks if current file is smaller
    for (size_t i = currentCount;
         i < originalCount;
         i++) {

        result.changedBlockIndices.push_back(i);
    }


    // Step 9: Final root comparison
    result.integrityMatch =
        (result.originalRoot == result.currentRoot);

    if (result.integrityMatch) {

        result.message =
            "SUCCESS: File integrity verified. "
            "No modifications detected.";

        result.changedBlockIndices.clear();

    }
    else {

        result.message =
            "WARNING: File tampering detected. "
            "Modified blocks have been identified.";
    }

    return true;
}
