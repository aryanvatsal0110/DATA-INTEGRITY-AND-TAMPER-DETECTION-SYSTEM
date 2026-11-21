
#include "TemperDProcessor.h"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <vector>
#include <system_error>

namespace fs = std::filesystem;


// --------------------------------------------------
// PART 1: Generate unique file ID
// --------------------------------------------------

std::string TemperDProcessor::createFileId() const {

    const auto now =
        std::chrono::system_clock::now();

    const auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();

    return "TPD-" + std::to_string(milliseconds);
}


// --------------------------------------------------
// PART 2: Build original Merkle Root
// --------------------------------------------------

bool TemperDProcessor::buildMerkleRoot(
    const std::vector<std::string>& hashes,
    std::string& root
) const {

    root.clear();

    MerkleTree tree;

    if (!tree.buildTree(hashes)) {
        return false;
    }

    root = tree.getRootHash();

    return root.size() == 64;
}


// --------------------------------------------------
// PART 3: Register uploaded file
// --------------------------------------------------

bool TemperDProcessor::registerFile(
    const std::string& uploadedFilePath,
    const std::string& storageRoot,
    RegistrationResult& result
) const {

    result = RegistrationResult();

    // Validate input
    if (uploadedFilePath.empty() || storageRoot.empty()) {

        result.message = "Invalid registration paths.";

        return false;
    }

    std::error_code error;

    if (!fs::is_regular_file(uploadedFilePath, error) || error) {

        result.message = "Uploaded file does not exist.";

        return false;
    }

    // Step 1: Profile file
    FileProfile profile;

    if (!profiler.profileFile(uploadedFilePath, profile)) {

        result.message = "File profiling failed.";

        return false;
    }

    // Step 2: Read file
    std::string fileData;

    if (!fileHandler.readFile(uploadedFilePath, fileData)) {

        result.message = "Unable to read uploaded file.";

        return false;
    }

    if (fileData.size() != profile.fileSize) {

        result.message = "File changed during registration.";

        return false;
    }

    // Step 3: Partition file
    std::vector<Block> blocks;

    PartitionConfig config;

    partitioner.partitionFile(
        fileData,
        blocks,
        config
    );

    if (blocks.empty() ||
        blocks.size() != config.totalBlocks ||
        config.blockSize == 0) {

        result.message = "File partitioning failed.";

        return false;
    }

    // Step 4: Generate SHA-256 block hashes
    std::vector<std::string> hashes;

    if (!hashGenerator.generateBlockHashes(
            blocks,
            hashes
        )) {

        result.message = "Block hashing failed.";

        return false;
    }

    // Step 5: Construct Merkle Tree
    std::string root;

    if (!buildMerkleRoot(hashes, root)) {

        result.message = "Merkle Tree construction failed.";

        return false;
    }

    // Step 6: Prepare metadata
    RootManager manager;

    if (!manager.setRootMetadata(
            root,
            fileData.size(),
            config.blockSize,
            config.totalBlocks,
            TreeType::STANDARD,
            0
        )) {

        result.message = "Root metadata creation failed.";

        return false;
    }

    // Step 7: Prepare storage directories
    const fs::path rootDirectory(storageRoot);

    const fs::path originalDirectory =
        rootDirectory / "original_files";

    const fs::path baselineDirectory =
        rootDirectory / "baselines";

    fs::create_directories(originalDirectory, error);

    if (error) {

        result.message = "Unable to create original file directory.";

        return false;
    }

    fs::create_directories(baselineDirectory, error);

    if (error) {

        result.message = "Unable to create baseline directory.";

        return false;
    }

    // Step 8: Create unique file ID
    std::string fileId = createFileId();

    // Preserve the original extension
    std::string extension =
        fs::path(uploadedFilePath).extension().string();

    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    const fs::path originalPath =
        originalDirectory / (fileId + extension);

    const fs::path baselinePath =
        baselineDirectory / (fileId + ".tdb");

    // Prevent accidental overwrite
    if (fs::exists(originalPath) ||
        fs::exists(baselinePath)) {

        result.message = "File ID collision. Please retry.";

        return false;
    }

    // Step 9: Preserve original file
    fs::copy_file(
        uploadedFilePath,
        originalPath,
        fs::copy_options::none,
        error
    );

    if (error) {

        result.message = "Failed to preserve original file.";

        return false;
    }

    // Step 10: Save baseline
    if (!baselineStorage.saveBaseline(
            baselinePath.string(),
            *manager.getMetadata(),
            hashes
        )) {

        fs::remove(originalPath, error);

        result.message = "Failed to save original baseline.";

        return false;
    }

    // Step 11: Return registration details
    result.success = true;

    result.fileId = fileId;

    result.originalFilePath =
        originalPath.string();

    result.baselinePath =
        baselinePath.string();

    result.merkleRoot = root;

    result.fileSize = fileData.size();

    result.totalBlocks = config.totalBlocks;

    result.blockSize = config.blockSize;

    result.message =
        "File successfully registered and baseline created.";

    return true;
}


// --------------------------------------------------
// PART 4: Verify registered file
// --------------------------------------------------

bool TemperDProcessor::verifyRegisteredFile(
    const std::string& uploadedFilePath,
    const std::string& fileId,
    const std::string& storageRoot,
    TemperDVerificationResult& result
) const {

    result = TemperDVerificationResult();

    if (uploadedFilePath.empty() ||
        fileId.empty() ||
        storageRoot.empty()) {

        result.message = "Invalid verification input.";

        return false;
    }

    // Validate file ID before constructing paths
    if (fileId.size() > 64 ||
        fileId.find_first_not_of(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"
        ) != std::string::npos) {

        result.message = "Invalid file ID.";

        return false;
    }

    const fs::path rootDirectory(storageRoot);

    const fs::path baselinePath =
        rootDirectory / "baselines" / (fileId + ".tdb");

    // Load the baseline
    BaselineData baseline;

    if (!baselineStorage.loadBaseline(
            baselinePath.string(),
            baseline
        )) {

        result.message = "Original baseline not found or invalid.";

        return false;
    }

    // Locate preserved original file
    std::string extension;

    // Search the original file directory using the registered ID.
    const fs::path originalDirectory =
        rootDirectory / "original_files";

    std::error_code error;

    if (!fs::is_directory(originalDirectory, error) || error) {

        result.message = "Original file storage directory not found.";

        return false;
    }

    fs::path originalPath;

    for (const auto& entry :
         fs::directory_iterator(originalDirectory, error)) {

        if (error) {
            break;
        }

        if (!entry.is_regular_file()) {
            continue;
        }

        if (entry.path().stem().string() == fileId) {

            originalPath = entry.path();

            break;
        }
    }

    if (error || originalPath.empty()) {

        result.message = "Preserved original file not found.";

        return false;
    }

    // Step 1: Verify current file against baseline
    if (!integrityVerifier.verifyFile(
            uploadedFilePath,
            baselinePath.string(),
            result.verification
        )) {

        result.message = result.verification.message;

        return false;
    }

    // Step 2: Read current file to obtain its size
    std::string currentData;

    if (!fileHandler.readFile(
            uploadedFilePath,
            currentData
        )) {

        result.message = "Unable to read uploaded verification file.";

        return false;
    }

    // Step 3: Convert changed block indices into byte ranges
    if (!tamperDetector.detectChanges(
            result.verification,
            baseline.metadata,
            currentData.size(),
            result.localization
        )) {

        result.message = result.localization.message;

        return false;
    }

    // Step 4: Perform text-level comparison for TXT files
    extension = originalPath.extension().string();

    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    if (extension == ".txt") {

        if (!fileComparator.compareFiles(
                originalPath.string(),
                uploadedFilePath,
                result.comparison
            )) {

            result.message = result.comparison.message;

            return false;
        }
    }

    // Other formats retain block-level localization for now.
    // CSV-specific comparison will be added separately.

    // Step 5: Prepare JSON report
    ReportInput reportInput;

    reportInput.originalFilePath =
        originalPath.string();

    reportInput.currentFilePath =
        uploadedFilePath;

    reportInput.reportDirectory =
        (rootDirectory / "reports").string();

    reportInput.currentFileSize =
        currentData.size();

    reportInput.verification =
        result.verification;

    reportInput.metadata =
        baseline.metadata;

    reportInput.localization =
        result.localization;

    reportInput.comparison =
        result.comparison;

    // Step 6: Generate report
    if (!reportGenerator.generateJSONReport(
            reportInput,
            result.report
        )) {

        result.message = result.report.message;

        return false;
    }

    result.success = true;

    result.message =
        result.verification.message + " " +
        result.localization.message + " " +
        result.report.message;

    return true;
}
