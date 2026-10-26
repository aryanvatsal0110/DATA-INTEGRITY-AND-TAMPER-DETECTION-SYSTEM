
#include "input/InputHandler.h"
#include "binary/BytePartitioner.h"
#include "binary/ByteHasher.h"
#include "tree/BinaryMerkleTree.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

std::string buildRoot(const std::vector<std::uint8_t>& bytes)
{
    const auto byteBlocks = partitionBytes(bytes, 64);
    const auto blockHashes = hashByteBlocks(byteBlocks);

    std::vector<Block> treeBlocks;
    treeBlocks.reserve(blockHashes.size());

    for (std::size_t i = 0; i < blockHashes.size(); ++i)
    {
        Block block;
        block.blockID = static_cast<int>(i);
        block.records.push_back(blockHashes[i]);
        treeBlocks.push_back(block);
    }

    BinaryMerkleTree tree;
    tree.build(treeBlocks);

    return tree.getRootHash();
}

int main()
{
    auto originalBytes = readFileBytes("data/sample.txt");

    if (originalBytes.empty())
    {
        std::cerr << "FAIL: Input file is empty or could not be read.\n";
        return 1;
    }

    // Build the root for the original file.
    const std::string originalRoot = buildRoot(originalBytes);

    // Modify one byte in memory; do not change the actual file.
    auto modifiedBytes = originalBytes;
    modifiedBytes[0] ^= 0x01;

    // Build the root for the modified data.
    const std::string modifiedRoot = buildRoot(modifiedBytes);

    std::cout << "Original file size: "
              << originalBytes.size() << " bytes\n";
    std::cout << "Original Merkle root: "
              << originalRoot << '\n';
    std::cout << "Modified Merkle root: "
              << modifiedRoot << '\n';

    if (originalRoot.empty() || modifiedRoot.empty())
    {
        std::cerr << "FAIL: Could not generate Merkle roots.\n";
        return 1;
    }

    if (originalRoot != modifiedRoot)
    {
        std::cout << "PASS: Tampering detected; Merkle roots differ.\n";
        return 0;
    }

    std::cerr << "FAIL: Merkle roots unexpectedly match.\n";
    return 1;
}
