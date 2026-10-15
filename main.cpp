#include "input/InputHandler.h"
#include "profiling/DataProfiler.h"
#include "partitioning/Partitioner.h"
#include "tree/BinaryMerkleTree.h"
#include "hashing/SHA256.h"
#include "proof/Merkleproof.h"
#include "verification/Verifier.h"
#include "detection/TamperDetector.h"
#include "localization/TamperLocalizer.h"
#include "report/TamperReport.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static std::string hashBlockData(const Block& block)
{
    std::string combinedData;

    for (const std::string& record : block.records)
        combinedData += record;

    return generateSHA256(combinedData);
}

static bool loadBaseline(
    const std::string& filename,
    std::string& trustedRoot,
    std::vector<std::string>& trustedLeafHashes)
{
    std::ifstream file(filename);

    if (!file)
        return false;

    std::size_t count = 0;

    if (!std::getline(file, trustedRoot) ||
        !(file >> count))
        return false;

    file.ignore(10000, '\n');

    trustedLeafHashes.clear();

    for (std::size_t i = 0; i < count; ++i)
    {
        std::string hash;

        if (!std::getline(file, hash) || hash.empty())
            return false;

        trustedLeafHashes.push_back(hash);
    }

    return !trustedRoot.empty();
}

static bool saveBaseline(
    const std::string& filename,
    const std::string& root,
    const std::vector<std::string>& leafHashes)
{
    std::ofstream file(filename, std::ios::trunc);

    if (!file)
        return false;

    file << root << '\n';
    file << leafHashes.size() << '\n';

    for (const std::string& hash : leafHashes)
        file << hash << '\n';

    return static_cast<bool>(file);
}

int main()
{
    const std::string inputFile = "data/sample.txt";
    const std::string baselineFile = "trusted_baseline.txt";
    const std::string reportFile = "tamper_report.txt";

    std::cout << "\n========================================\n";
    std::cout << " DATA INTEGRITY AND TAMPER DETECTION\n";
    std::cout << "========================================\n";

    // 1. Read the actual file.
    const std::vector<std::string> records = readDataset(inputFile);

    if (records.empty())
    {
        std::cerr << "ERROR: Dataset is empty or could not be read.\n";
        return 1;
    }

    std::cout << "[1/7] Input reading: PASSED\n";

    // 2. Profile and partition the actual data.
    const DataProfile profile = profileDataset(inputFile, records);
    displayProfile(profile);

    const std::vector<Block> blocks = adaptivePartition(records, profile);

    if (blocks.empty())
    {
        std::cerr << "ERROR: No blocks were generated.\n";
        return 1;
    }

    std::cout << "\n[2/7] Partitioning: PASSED\n";
    std::cout << "Total blocks: " << blocks.size() << '\n';

    // Hash the current blocks using the same rule as the tree.
    std::vector<std::string> currentLeafHashes;

    for (const Block& block : blocks)
        currentLeafHashes.push_back(hashBlockData(block));

    // 3. Construct the Merkle tree from the current file.
    BinaryMerkleTree currentTree;
    currentTree.build(blocks);

    const std::string currentRoot = currentTree.getRootHash();

    if (currentRoot.empty())
    {
        std::cerr << "ERROR: Could not construct Merkle tree.\n";
        return 1;
    }

    std::cout << "\n[3/7] Merkle tree construction: PASSED\n";
    std::cout << "Current root: " << currentRoot << '\n';

    // 4. If no baseline exists, establish it once.
    std::ifstream baselineCheck(baselineFile);
    const bool baselineExists = baselineCheck.good();
    baselineCheck.close();

    if (!baselineExists)
    {
        if (!saveBaseline(
                baselineFile, currentRoot, currentLeafHashes))
        {
            std::cerr << "ERROR: Could not save trusted baseline.\n";
            return 1;
        }

        std::cout << "\nTRUSTED BASELINE CREATED\n";
        std::cout << "Baseline file: " << baselineFile << '\n';
        std::cout << "Root: " << currentRoot << '\n';
        std::cout << "Run the program again to verify the file.\n";
        std::cout << "Edit " << inputFile
                  << " before that run to test tamper detection.\n";

        return 0;
    }

    // 5. Load the saved trusted root and trusted leaf hashes.
    std::string trustedRoot;
    std::vector<std::string> trustedLeafHashes;

    if (!loadBaseline(
            baselineFile, trustedRoot, trustedLeafHashes))
    {
        std::cerr << "ERROR: Baseline is invalid or unreadable.\n";
        std::cerr << "Do not delete or replace it automatically.\n";
        return 1;
    }

    std::cout << "Trusted root: " << trustedRoot << '\n';

    // Generate a proof for a current block and verify against
    // the SAVED trusted root, not a newly generated root.
    const MerkleProof proof =
        generateMerkleProof(currentLeafHashes, 0);

    const bool proofValid =
        proof.blockID != -1 &&
        verifyMerkleProof(proof, trustedRoot);

    std::cout << "[4/7] Trusted-root proof verification: "
              << (proofValid ? "PASSED" : "FAILED / MISMATCH")
              << '\n';

    // 6. Compare the current root with the saved root.
    const bool tampered = detectTampering(trustedRoot, currentRoot);

    std::cout << "\n[5/7] Tamper detection: "
              << (tampered ? "TAMPER DETECTED" : "NO MISMATCH")
              << '\n';

    if (tampered)
        std::cout << "The current file differs from the trusted baseline.\n";
    else
        std::cout << "The current root matches the trusted baseline.\n";

    // 7. Localize changes and write the report.
    const std::vector<int> affectedBlocks =
        locateTamperedBlocks(blocks, trustedLeafHashes);

    std::cout << "[6/7] Tamper localization: ";

    if (tampered && affectedBlocks.empty())
        std::cout << "ROOT MISMATCH; NO BLOCK IDENTIFIED\n";
    else
        std::cout << "COMPLETED\n";

    for (int id : affectedBlocks)
    {
        if (id == -1)
            std::cout << "Block layout changed.\n";
        else
            std::cout << "Affected block ID: " << id << '\n';
    }

    generateTamperReport(
        trustedRoot, currentRoot, affectedBlocks, reportFile);

    std::cout << "[7/7] Report generation: " << reportFile << '\n';

    std::cout << "\n========================================\n";
    std::cout << (tampered
        ? " INTEGRITY MISMATCH DETECTED\n"
        : " DATA INTEGRITY VERIFIED\n");
    std::cout << "========================================\n";

    // A mismatch is a detection result, not a program crash.
    return tampered ? 2 : 0;
}