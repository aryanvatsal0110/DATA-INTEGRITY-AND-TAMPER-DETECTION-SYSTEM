
#include "input/InputHandler.h"
#include "binary/BytePartitioner.h"
#include "binary/ByteHasher.h"
#include "tree/BinaryMerkleTree.h"

#include <iostream>
#include <vector>
#include <string>

int main()
{
    const auto bytes = readFileBytes("data/sample.txt");

    if (bytes.empty())
    {
        std::cerr << "FAIL: Input is empty or unreadable.\n";
        return 1;
    }

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

    const std::string rootHash = tree.getRootHash();

    if (rootHash.size() != 64)
    {
        std::cerr << "FAIL: Merkle root hash is invalid.\n";
        return 1;
    }

    std::cout << "Original file bytes: " << bytes.size() << '\n';
    std::cout << "Binary blocks: " << byteBlocks.size() << '\n';
    std::cout << "Merkle root: " << rootHash << '\n';
    std::cout << "PASS: Binary block hashes entered the Merkle Tree.\n";

    return 0;
}
