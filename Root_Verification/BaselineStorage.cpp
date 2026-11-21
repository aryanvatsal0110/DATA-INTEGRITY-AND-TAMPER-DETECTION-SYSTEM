
#include "BaselineStorage.h"

#include <fstream>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include <utility>

namespace {

    constexpr char MAGIC[] = "TPDBASE1";
    constexpr uint32_t FORMAT_VERSION = 1;

    constexpr uint64_t MAX_BLOCK_COUNT = 10000000;


    // Write a 32-bit integer in little-endian format
    bool writeU32(std::ofstream& file, uint32_t value) {

        unsigned char bytes[4];

        for (int i = 0; i < 4; i++) {
            bytes[i] = static_cast<unsigned char>(
                value >> (i * 8)
            );
        }

        file.write(
            reinterpret_cast<const char*>(bytes),
            sizeof(bytes)
        );

        return file.good();
    }


    // Read a 32-bit integer
    bool readU32(std::ifstream& file, uint32_t& value) {

        unsigned char bytes[4];

        file.read(
            reinterpret_cast<char*>(bytes),
            sizeof(bytes)
        );

        if (!file) {
            return false;
        }

        value = 0;

        for (int i = 0; i < 4; i++) {
            value |= static_cast<uint32_t>(bytes[i]) << (i * 8);
        }

        return true;
    }


    // Write a 64-bit integer in little-endian format
    bool writeU64(std::ofstream& file, uint64_t value) {

        unsigned char bytes[8];

        for (int i = 0; i < 8; i++) {
            bytes[i] = static_cast<unsigned char>(
                value >> (i * 8)
            );
        }

        file.write(
            reinterpret_cast<const char*>(bytes),
            sizeof(bytes)
        );

        return file.good();
    }


    // Read a 64-bit integer
    bool readU64(std::ifstream& file, uint64_t& value) {

        unsigned char bytes[8];

        file.read(
            reinterpret_cast<char*>(bytes),
            sizeof(bytes)
        );

        if (!file) {
            return false;
        }

        value = 0;

        for (int i = 0; i < 8; i++) {
            value |= static_cast<uint64_t>(bytes[i]) << (i * 8);
        }

        return true;
    }


    // Validate a SHA-256 hexadecimal hash
    bool isValidHash(const std::string& hash) {

        if (hash.size() != 64) {
            return false;
        }

        for (unsigned char ch : hash) {

            if (!((ch >= '0' && ch <= '9') ||
                  (ch >= 'a' && ch <= 'f') ||
                  (ch >= 'A' && ch <= 'F'))) {

                return false;
            }
        }

        return true;
    }


    // Validate metadata using RootManager
    bool isValidMetadata(const RootMetadata& metadata) {

        RootManager manager;

        return manager.setRootMetadata(
            metadata.merkleRoot,
            metadata.originalFileSize,
            metadata.blockSize,
            metadata.totalBlocks,
            metadata.treeType,
            metadata.groupSize
        );
    }


    // Check whether block count matches file size and block size
    bool hasCorrectBlockCount(const RootMetadata& metadata) {

        uint64_t expectedBlocks;

        if (metadata.originalFileSize == 0) {
            expectedBlocks = 1;
        }
        else {
            expectedBlocks =
                metadata.originalFileSize / metadata.blockSize +
                (metadata.originalFileSize % metadata.blockSize != 0);
        }

        return expectedBlocks == metadata.totalBlocks;
    }

}


// Validate baseline before saving
bool BaselineStorage::validateBaseline(
    const RootMetadata& metadata,
    const std::vector<std::string>& blockHashes
) const {

    if (!isValidMetadata(metadata)) {
        return false;
    }

    if (!hasCorrectBlockCount(metadata)) {
        return false;
    }

    if (blockHashes.size() != metadata.totalBlocks) {
        return false;
    }

    if (blockHashes.empty() ||
        blockHashes.size() > MAX_BLOCK_COUNT) {
        return false;
    }

    for (const std::string& hash : blockHashes) {

        if (!isValidHash(hash)) {
            return false;
        }
    }

    return true;
}


// Save baseline to persistent storage
bool BaselineStorage::saveBaseline(
    const std::string& filePath,
    const RootMetadata& metadata,
    const std::vector<std::string>& blockHashes
) const {

    if (!validateBaseline(metadata, blockHashes)) {
        return false;
    }

    if (filePath.empty()) {
        return false;
    }

    std::ofstream file(
        filePath,
        std::ios::binary | std::ios::trunc
    );

    if (!file.is_open()) {
        return false;
    }

    // Write file signature
    file.write(MAGIC, sizeof(MAGIC) - 1);

    if (!file.good()) {
        return false;
    }

    // Write format version
    if (!writeU32(file, FORMAT_VERSION)) {
        return false;
    }

    // Write Merkle Root
    file.write(
        metadata.merkleRoot.data(),
        static_cast<std::streamsize>(metadata.merkleRoot.size())
    );

    if (!file.good()) {
        return false;
    }

    // Write original file metadata
    if (!writeU64(file, metadata.originalFileSize) ||
        !writeU64(file, metadata.blockSize) ||
        !writeU64(file, metadata.totalBlocks)) {

        return false;
    }

    // Write tree type
    uint32_t treeType =
        (metadata.treeType == TreeType::STANDARD) ? 0 : 1;

    if (!writeU32(file, treeType)) {
        return false;
    }

    // Write hierarchical group size
    if (!writeU64(file, metadata.groupSize)) {
        return false;
    }

    // Write hash count
    if (!writeU64(file, blockHashes.size())) {
        return false;
    }

    // Write all original block hashes
    for (const std::string& hash : blockHashes) {

        file.write(
            hash.data(),
            static_cast<std::streamsize>(hash.size())
        );

        if (!file.good()) {
            return false;
        }
    }

    file.flush();

    return file.good();
}


// Load baseline from persistent storage
bool BaselineStorage::loadBaseline(
    const std::string& filePath,
    BaselineData& baseline
) const {

    baseline = BaselineData();

    if (filePath.empty()) {
        return false;
    }

    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // Read and validate file signature
    char magic[sizeof(MAGIC) - 1];

    file.read(magic, sizeof(magic));

    if (!file ||
        std::string(magic, sizeof(magic)) !=
        std::string(MAGIC, sizeof(MAGIC) - 1)) {

        return false;
    }

    // Read format version
    uint32_t version;

    if (!readU32(file, version) ||
        version != FORMAT_VERSION) {

        return false;
    }

    BaselineData loaded;

    // Read Merkle Root
    loaded.metadata.merkleRoot.resize(64);

    file.read(
        &loaded.metadata.merkleRoot[0],
        64
    );

    if (!file) {
        return false;
    }

    // Read metadata fields
    uint64_t fileSize;
    uint64_t blockSize;
    uint64_t totalBlocks;
    uint64_t groupSize;
    uint64_t hashCount;

    uint32_t treeType;

    if (!readU64(file, fileSize) ||
        !readU64(file, blockSize) ||
        !readU64(file, totalBlocks) ||
        !readU32(file, treeType) ||
        !readU64(file, groupSize) ||
        !readU64(file, hashCount)) {

        return false;
    }

    // Validate integer conversions
    const uint64_t maxSize =
        static_cast<uint64_t>(
            std::numeric_limits<size_t>::max()
        );

    if (fileSize > maxSize ||
        blockSize > maxSize ||
        totalBlocks > maxSize ||
        groupSize > maxSize ||
        hashCount > MAX_BLOCK_COUNT) {

        return false;
    }

    if (treeType > 1) {
        return false;
    }

    // Restore metadata
    loaded.metadata.originalFileSize =
        static_cast<size_t>(fileSize);

    loaded.metadata.blockSize =
        static_cast<size_t>(blockSize);

    loaded.metadata.totalBlocks =
        static_cast<size_t>(totalBlocks);

    loaded.metadata.groupSize =
        static_cast<size_t>(groupSize);

    loaded.metadata.treeType =
        (treeType == 0)
        ? TreeType::STANDARD
        : TreeType::HIERARCHICAL;

    // Validate metadata and hash count before allocation
    if (!isValidMetadata(loaded.metadata) ||
        !hasCorrectBlockCount(loaded.metadata) ||
        hashCount != loaded.metadata.totalBlocks ||
        hashCount == 0) {

        return false;
    }

    // Read original block hashes
    loaded.blockHashes.reserve(
        static_cast<size_t>(hashCount)
    );

    for (uint64_t i = 0; i < hashCount; i++) {

        std::string hash(64, '\0');

        file.read(&hash[0], 64);

        if (!file || !isValidHash(hash)) {
            return false;
        }

        loaded.blockHashes.push_back(std::move(hash));
    }

    // Reject unexpected trailing data
    char extra;

    if (file.get(extra)) {
        return false;
    }

    if (!file.eof()) {
        return false;
    }

    if (!validateBaseline(
            loaded.metadata,
            loaded.blockHashes
        )) {

        return false;
    }

    baseline = std::move(loaded);

    return true;
}
