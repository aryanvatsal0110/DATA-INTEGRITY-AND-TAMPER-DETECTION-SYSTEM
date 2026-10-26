
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

int main()
{
    const std::string filePath = "data/sample.txt";
    const std::size_t blockSize = 64;

    // Step 1: Read the actual file as raw bytes.
    std::ifstream file(filePath, std::ios::binary);

    if (!file)
    {
        std::cerr << "ERROR: Cannot open " << filePath << '\n';
        return 1;
    }

    std::vector<std::uint8_t> bytes(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    if (bytes.empty())
    {
        std::cerr << "ERROR: File is empty.\n";
        return 1;
    }

    std::cout << "Original file bytes: "
              << bytes.size() << '\n';

    // Step 2: Partition the file into fixed-size byte blocks.
    const auto blocks = partitionBytes(bytes, blockSize);

    std::cout << "Binary blocks: "
              << blocks.size() << '\n';

    // Step 3: Hash every block using SHA-256.
    const auto leafHashes = hashByteBlocks(blocks);

    if (leafHashes.empty())
    {
        std::cerr << "ERROR: No block hashes generated.\n";
        return 1;
    }

    std::cout << "Block hashes generated: "
              << leafHashes.size() << '\n';

    // Step 4: Build a proof for the first block.
    const MerkleProof proof =
        generateMerkleProof(
            leafHashes,
            0
        );

    if (proof.blockID < 0)
    {
        std::cerr << "ERROR: Proof generation failed.\n";
        return 1;
    }

    // Step 5: Reconstruct the root using the proof.
    const std::string proofRoot =
        regenerateRoot(proof);

    // Calculate the expected root using the same tree structure.
    // This function uses midpoint splitting, matching the
    // existing BinaryMerkleTree implementation.
    auto calculateRoot =
        [&](const auto& self,
            int start,
            int end) -> std::string
    {
        if (start == end)
            return leafHashes[start];

        const int middle = start + (end - start) / 2;

        const std::string left =
            self(self, start, middle);

        const std::string right =
            self(self, middle + 1, end);

        return generateSHA256(left + right);
    };

    const std::string expectedRoot =
        calculateRoot(
            calculateRoot,
            0,
            static_cast<int>(leafHashes.size()) - 1
        );

    std::cout << "Expected Merkle root: "
              << expectedRoot << '\n';

    std::cout << "Proof-reconstructed root: "
              << proofRoot << '\n';

    // Step 6: Verify the proof against the expected root.
    const bool valid =
        verifyMerkleProof(proof, expectedRoot);

    if (valid && proofRoot == expectedRoot)
    {
        std::cout
            << "PASS: Binary block Merkle proof verified.\n";
        return 0;
    }

    std::cout
        << "FAIL: Merkle proof verification failed.\n";

    return 2;
}
