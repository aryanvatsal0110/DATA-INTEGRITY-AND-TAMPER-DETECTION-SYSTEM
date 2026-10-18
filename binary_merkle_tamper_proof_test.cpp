
#include "binary/BytePartitioner.h"
#include "binary/ByteHasher.h"
#include "proof/Merkleproof.h"
#include "verification/Verifier.h"
#include "hashing/SHA256.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

std::string calculateRoot(const std::vector<std::string>& hashes)
{
    if (hashes.empty())
        return "";

    if (hashes.size() == 1)
        return hashes[0];

    const int middle =
        static_cast<int>(hashes.size() / 2) - 1;

    std::vector<std::string> left(
        hashes.begin(), hashes.begin() + middle + 1);

    std::vector<std::string> right(
        hashes.begin() + middle + 1, hashes.end());

    return generateSHA256(
        calculateRoot(left) + calculateRoot(right));
}

int main()
{
    const std::string filePath = "data/sample.txt";
    const std::size_t blockSize = 64;

    std::ifstream file(filePath, std::ios::binary);

    if (!file)
    {
        std::cerr << "ERROR: Cannot open input file.\n";
        return 1;
    }

    std::vector<std::uint8_t> originalBytes(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    if (originalBytes.empty())
    {
        std::cerr << "ERROR: Input file is empty.\n";
        return 1;
    }

    // Create the original trusted reference.
    const auto originalBlocks =
        partitionBytes(originalBytes, blockSize);

    const auto originalHashes =
        hashByteBlocks(originalBlocks);

    const std::string trustedRoot =
        calculateRoot(originalHashes);

    // Generate a proof for block zero from the original data.
    const MerkleProof originalProof =
        generateMerkleProof(originalHashes, 0);

    if (originalProof.blockID < 0)
    {
        std::cerr << "ERROR: Could not generate proof.\n";
        return 1;
    }

    std::cout << "Trusted root: " << trustedRoot << '\n';

    // Tamper with one byte in the first block.
    auto modifiedBytes = originalBytes;
    modifiedBytes[0] ^= 0x01;

    const auto modifiedBlocks =
        partitionBytes(modifiedBytes, blockSize);

    const auto modifiedHashes =
        hashByteBlocks(modifiedBlocks);

    const std::string modifiedRoot =
        calculateRoot(modifiedHashes);

    // Build a proof from the modified block hashes.
    const MerkleProof modifiedProof =
        generateMerkleProof(modifiedHashes, 0);

    const bool originalAccepted =
        verifyMerkleProof(originalProof, trustedRoot);

    const bool modifiedAccepted =
        verifyMerkleProof(modifiedProof, trustedRoot);

    std::cout << "Modified root: " << modifiedRoot << '\n';

    if (!originalAccepted)
    {
        std::cerr << "FAIL: Original proof was rejected.\n";
        return 2;
    }

    std::cout << "PASS: Original proof accepted.\n";

    if (modifiedRoot == trustedRoot || modifiedAccepted)
    {
        std::cerr
            << "FAIL: Tampered data was not rejected.\n";
        return 3;
    }

    std::cout
        << "PASS: Tampered data rejected by trusted root.\n";

    return 0;
}
