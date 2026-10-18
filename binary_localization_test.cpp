
#include "binary/BytePartitioner.h"
#include "binary/ByteHasher.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main()
{
    const std::string filePath = "data/sample.txt";
    const std::size_t blockSize = 64;

    // Read the file as raw bytes.
    std::ifstream file(filePath, std::ios::binary);

    if (!file)
    {
        std::cerr << "ERROR: Cannot open input file.\n";
        return 1;
    }

    const std::vector<std::uint8_t> originalBytes(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    if (originalBytes.empty())
    {
        std::cerr << "ERROR: Input file is empty.\n";
        return 1;
    }

    // Partition and hash the original data.
    const auto originalBlocks =
        partitionBytes(originalBytes, blockSize);

    const auto trustedHashes =
        hashByteBlocks(originalBlocks);

    // Simulate tampering in memory without editing the real file.
    auto modifiedBytes = originalBytes;

    const std::size_t changedPosition = 130;
    modifiedBytes[changedPosition] ^= 0x01;

    const auto modifiedBlocks =
        partitionBytes(modifiedBytes, blockSize);

    const auto currentHashes =
        hashByteBlocks(modifiedBlocks);

    std::cout << "Original file size: "
              << originalBytes.size() << " bytes\n";

    std::cout << "Block size: "
              << blockSize << " bytes\n";

    std::cout << "Changed byte position: "
              << changedPosition << '\n';

    // Compare block hashes to localize changes.
    if (trustedHashes.size() != currentHashes.size())
    {
        std::cout << "DETECTED: Number of blocks changed.\n";
    }

    bool foundTampering = false;

    const std::size_t count =
        trustedHashes.size() < currentHashes.size()
            ? trustedHashes.size()
            : currentHashes.size();

    for (std::size_t i = 0; i < count; ++i)
    {
        if (trustedHashes[i] != currentHashes[i])
        {
            std::cout << "TAMPERED BLOCK: " << i << '\n';
            foundTampering = true;
        }
    }

    if (trustedHashes.size() != currentHashes.size())
        foundTampering = true;

    if (!foundTampering)
    {
        std::cerr << "FAIL: Tampering was not detected.\n";
        return 2;
    }

    const std::size_t expectedBlock =
        changedPosition / blockSize;

    std::cout << "Expected affected block: "
              << expectedBlock << '\n';

    if (trustedHashes.size() == currentHashes.size() &&
        trustedHashes[expectedBlock] !=
            currentHashes[expectedBlock])
    {
        std::cout
            << "PASS: Correct block localized.\n";
        return 0;
    }

    std::cerr
        << "FAIL: Expected block was not identified correctly.\n";
    return 3;
}
