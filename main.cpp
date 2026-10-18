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

#include "binary/BytePartitioner.h"
#include "binary/ByteHasher.h"

#include <algorithm>
#include <cstdint>
#include <iterator>

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

static int runTextWorkflow()
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


constexpr std::size_t BINARY_BLOCK_SIZE = 64;

struct BinaryIntegrityData
{
    std::vector<std::string> blockHashes;
    std::string root;
    std::size_t fileSize = 0;
};

static std::string calculateBinaryRoot(
    const std::vector<std::string>& hashes)
{
    if (hashes.empty())
        return "";

    if (hashes.size() == 1)
        return hashes[0];

    const std::size_t middle = hashes.size() / 2;

    std::vector<std::string> left(
        hashes.begin(), hashes.begin() + middle);

    std::vector<std::string> right(
        hashes.begin() + middle, hashes.end());

    return generateSHA256(
        calculateBinaryRoot(left) +
        calculateBinaryRoot(right));
}

static bool readBinaryFile(
    const std::string& path,
    std::vector<std::uint8_t>& bytes)
{
    std::ifstream file(path, std::ios::binary);

    if (!file)
        return false;

    bytes.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());

    return file.eof() || file.good();
}

static BinaryIntegrityData calculateBinaryIntegrity(
    const std::vector<std::uint8_t>& bytes)
{
    BinaryIntegrityData result;
    result.fileSize = bytes.size();

    const auto blocks =
        partitionBytes(bytes, BINARY_BLOCK_SIZE);

    result.blockHashes = hashByteBlocks(blocks);

    if (bytes.empty())
        result.root = generateSHA256("");
    else
        result.root = calculateBinaryRoot(result.blockHashes);

    return result;
}

static bool saveBinaryBaseline(
    const BinaryIntegrityData& data,
    const std::string& path)
{
    std::ofstream file(path, std::ios::trunc);

    if (!file)
        return false;

    file << data.fileSize << '\n';
    file << data.root << '\n';
    file << data.blockHashes.size() << '\n';

    for (const auto& hash : data.blockHashes)
        file << hash << '\n';

    return file.good();
}

static bool loadBinaryBaseline(
    BinaryIntegrityData& data,
    const std::string& path)
{
    std::ifstream file(path);

    if (!file)
        return false;

    std::size_t count = 0;

    if (!(file >> data.fileSize) ||
        !(file >> data.root) ||
        !(file >> count))
        return false;

    if (data.root.empty() ||
        count > data.fileSize ||
        (data.fileSize == 0 && count != 0) ||
        (data.fileSize != 0 && count == 0))
        return false;

    data.blockHashes.clear();

    for (std::size_t i = 0; i < count; ++i)
    {
        std::string hash;

        if (!(file >> hash) || hash.size() != 64)
            return false;

        data.blockHashes.push_back(hash);
    }

    if (data.blockHashes.size() != count)
        return false;

    if (data.fileSize != 0)
    {
        const std::size_t expectedCount =
            data.fileSize / BINARY_BLOCK_SIZE +
            (data.fileSize % BINARY_BLOCK_SIZE != 0);

        if (count != expectedCount)
            return false;
    }

    return true;
}

static int runBinaryWorkflow(
    const std::string& mode,
    const std::string& inputPath)
{
    // Preserve the same baseline naming convention as the
    // already-tested binary_baseline_test.cpp.
    const std::string pathHash = generateSHA256(inputPath);

    const std::string baselinePath =
        "binary_baseline_" + pathHash + ".txt";

    const std::string reportPath =
        "binary_report_" + pathHash + ".txt";

    std::vector<std::uint8_t> bytes;

    if (!readBinaryFile(inputPath, bytes))
    {
        std::cerr << "ERROR: Could not read file: "
                  << inputPath << '\n';
        return 1;
    }

    const BinaryIntegrityData current =
        calculateBinaryIntegrity(bytes);

    if (current.root.empty())
    {
        std::cerr << "ERROR: Could not calculate file root.\n";
        return 1;
    }

    if (mode == "create")
    {
        // Never silently overwrite an existing trusted baseline.
        std::ifstream existing(baselinePath);

        if (existing.good())
        {
            std::cerr
                << "ERROR: A baseline already exists for this path.\n"
                << "Use verify, or deliberately archive the old "
                << "baseline before registering a new original.\n";
            return 1;
        }

        if (!saveBinaryBaseline(current, baselinePath))
        {
            std::cerr << "ERROR: Could not save baseline.\n";
            return 1;
        }

        std::cout << "Trusted baseline created.\n"
                  << "File: " << inputPath << '\n'
                  << "File size: " << current.fileSize << " bytes\n"
                  << "Block count: " << current.blockHashes.size()
                  << '\n'
                  << "Trusted root: " << current.root << '\n'
                  << "Baseline: " << baselinePath << '\n';

        return 0;
    }

    BinaryIntegrityData trusted;

    if (!loadBinaryBaseline(trusted, baselinePath))
    {
        std::cerr
            << "ERROR: Trusted baseline missing or invalid.\n"
            << "Register the known-good file first using create.\n";
        return 1;
    }

    const bool tampered =
        detectTampering(trusted.root, current.root);

    std::vector<int> affectedBlocks;

    const std::size_t sharedCount = std::min(
        trusted.blockHashes.size(),
        current.blockHashes.size());

    for (std::size_t i = 0; i < sharedCount; ++i)
    {
        if (trusted.blockHashes[i] != current.blockHashes[i])
            affectedBlocks.push_back(static_cast<int>(i));
    }

    if (trusted.blockHashes.size() != current.blockHashes.size())
        affectedBlocks.push_back(-1);

    std::cout << "File: " << inputPath << '\n'
              << "Trusted root: " << trusted.root << '\n'
              << "Current root: " << current.root << '\n';

    if (tampered)
        std::cout << "DETECTED: File integrity mismatch.\n";
    else
        std::cout << "PASS: File integrity verified.\n";

    for (int id : affectedBlocks)
    {
        if (id == -1)
            std::cout << "Block layout changed.\n";
        else
            std::cout << "TAMPERED BLOCK: " << id << '\n';
    }

    generateTamperReport(
        trusted.root,
        current.root,
        affectedBlocks,
        reportPath);

    std::cout << "Report: " << reportPath << '\n';

    return tampered ? 2 : 0;
}

int main(int argc, char* argv[])
{
    if (argc == 1)
        return runTextWorkflow();

    if (argc != 3 ||
        (std::string(argv[1]) != "create" &&
         std::string(argv[1]) != "verify"))
    {
        std::cerr
            << "Usage:\n"
            << "  " << argv[0] << '\n'
            << "  " << argv[0] << " create <file_path>\n"
            << "  " << argv[0] << " verify <file_path>\n";

        return 1;
    }

    return runBinaryWorkflow(argv[1], argv[2]);
}
