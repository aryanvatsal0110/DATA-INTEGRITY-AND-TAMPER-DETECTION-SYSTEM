
#include "binary/BytePartitioner.h"
#include "binary/ByteHasher.h"
#include "detection/TamperDetector.h"
#include "report/TamperReport.h"
#include "hashing/SHA256.h"


#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

constexpr std::size_t BLOCK_SIZE = 64;

struct FileIntegrityData
{
    std::vector<std::string> blockHashes;
    std::string root;
    std::size_t fileSize = 0;
};

std::string calculateRoot(const std::vector<std::string>& hashes)
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
        calculateRoot(left) + calculateRoot(right));
}

bool readBytes(
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

FileIntegrityData calculateIntegrity(
    const std::vector<std::uint8_t>& bytes)
{
    FileIntegrityData result;
    result.fileSize = bytes.size();

    const auto blocks = partitionBytes(bytes, BLOCK_SIZE);
    result.blockHashes = hashByteBlocks(blocks);
    result.root = calculateRoot(result.blockHashes);

    return result;
}

bool saveBaseline(
    const FileIntegrityData& data,
    const std::string& baselinePath)
{
    std::ofstream file(baselinePath);
    

    if (!file)
        return false;

    file << data.fileSize << '\n';
    file << data.root << '\n';
    file << data.blockHashes.size() << '\n';

    for (const auto& hash : data.blockHashes)
        file << hash << '\n';

    return file.good();
}

bool loadBaseline(
    FileIntegrityData& data,
    const std::string& baselinePath)
{
    std::ifstream file(baselinePath);
    

    if (!file)
        return false;

    std::size_t count = 0;

    if (!(file >> data.fileSize))
        return false;

    if (!(file >> data.root))
        return false;

    if (!(file >> count))
        return false;

    data.blockHashes.clear();

    for (std::size_t i = 0; i < count; ++i)
    {
        std::string hash;

        if (!(file >> hash))
            return false;

        data.blockHashes.push_back(hash);
    }

    return data.blockHashes.size() == count &&
           !data.root.empty();
}

/*int main(int argc, char* argv[])
{
    if (argc != 2 ||
        (std::string(argv[1]) != "create" &&
         std::string(argv[1]) != "verify"))
    {
        std::cout
            << "Usage:\n"
            << "  .\\binary_baseline_test.exe create\n"
            << "  .\\binary_baseline_test.exe verify\n";
        return 1;
    }

    //const std::string inputPath = "data/sample.txt";
    if (argc < 2) {
    std::cerr << "Usage: " << argv[0]
              << " <create|verify> <file_path>\n";
    return 1;
}

if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << " <create|verify> <file_path>\n";
    return 1;
}

const std::string inputPath = argv[2];*/

int main(int argc, char* argv[])
{
    if (argc != 3 ||
        (std::string(argv[1]) != "create" &&
         std::string(argv[1]) != "verify"))
    {
        std::cerr
            << "Usage:\n"
            << "  " << argv[0] << " create <file_path>\n"
            << "  " << argv[0] << " verify <file_path>\n";
        return 1;
    }

    const std::string mode = argv[1];
    const std::string inputPath = argv[2];
    //const std::string baselinePath = "binary_pdf_trusted_baseline.txt";

    //std::filesystem::create_directories("baselines");
/*     std::filesystem::create_directories("baselines");

    const std::string baselinePath = "baselines/" + generateSHA256(inputPath) + ".baseline";

    const std::string baselinePath =
    "baselines/" + generateSHA256(inputPath) + ".baseline"; */
    const std::string baselinePath =
    "binary_baseline_" + generateSHA256(inputPath) + ".txt";

    // Keep the existing code below this point unchanged.



    std::vector<std::uint8_t> bytes;

    if (!readBytes(inputPath, bytes) || bytes.empty())
    {
        std::cerr << "ERROR: Cannot read a non-empty input file.\n";
        return 1;
    }

    const FileIntegrityData current =
        calculateIntegrity(bytes);

    if (current.root.empty())
    {
        std::cerr << "ERROR: Root calculation failed.\n";
        return 1;
    }

    if (std::string(argv[1]) == "create")
    {
        if (!saveBaseline(current, baselinePath))
        {
            std::cerr << "ERROR: Cannot save trusted baseline.\n";
            return 1;
        }

        std::cout << "Trusted baseline created.\n";
        std::cout << "File size: " << current.fileSize << " bytes\n";
        std::cout << "Block count: "
                  << current.blockHashes.size() << '\n';
        std::cout << "Trusted root: " << current.root << '\n';
        return 0;
    }

    FileIntegrityData trusted;

    if (!loadBaseline(trusted, baselinePath))
    {
        std::cerr
            << "ERROR: Trusted baseline missing or invalid.\n"
            << "Run create on the original file first.\n";
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
    {
        // Mark layout changes as well as changed shared blocks.
        affectedBlocks.push_back(-1);
    }

    std::cout << "Trusted root: " << trusted.root << '\n';
    std::cout << "Current root: " << current.root << '\n';

    if (!tampered)
    {
        std::cout << "PASS: File integrity verified.\n";
    }
    else
    {
        std::cout << "DETECTED: File integrity mismatch.\n";
    }

    if (affectedBlocks.empty() && tampered)
        std::cout << "No individual changed block identified.\n";

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
        "binary_tamper_report.txt");

    return tampered ? 2 : 0;
}
