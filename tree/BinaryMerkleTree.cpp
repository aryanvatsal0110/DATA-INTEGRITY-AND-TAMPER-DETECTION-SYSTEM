#include "BinaryMerkleTree.h"

#include "../hashing/SHA256.h"

using namespace std;


// ==========================================
// CONSTRUCTOR
// ==========================================

BinaryMerkleTree::BinaryMerkleTree()
{
    root = nullptr;
}


// ==========================================
// DESTRUCTOR
// ==========================================

BinaryMerkleTree::~BinaryMerkleTree()
{
    deleteTree(root);
}


// ==========================================
// DELETE TREE
// ==========================================

void BinaryMerkleTree::deleteTree(
    MerkleNode* node)
{
    if (node == nullptr)
        return;

    deleteTree(node->left);
    deleteTree(node->right);

    delete node;
}


// ==========================================
// HASH A BLOCK
// ==========================================

string BinaryMerkleTree::hashBlock(
    const Block& block)
{
    string combinedData;

    for (const string& record : block.records)
    {
        combinedData += record;
    }

    return generateSHA256(combinedData);
}


// ==========================================
// BUILD BINARY MERKLE TREE
// ==========================================

MerkleNode* BinaryMerkleTree::buildTree(
    const vector<string>& hashes,
    int start,
    int end)
{
    if (start > end)
    {
        return nullptr;
    }


    // --------------------------------------
    // Leaf node
    // --------------------------------------

    if (start == end)
    {
        return new MerkleNode(
            hashes[start]
        );
    }


    // --------------------------------------
    // Find middle
    // --------------------------------------

    int middle =
        start + (end - start) / 2;


    // --------------------------------------
    // Build left subtree
    // --------------------------------------

    MerkleNode* left =
        buildTree(
            hashes,
            start,
            middle
        );


    // --------------------------------------
    // Build right subtree
    // --------------------------------------

    MerkleNode* right =
        buildTree(
            hashes,
            middle + 1,
            end
        );


    // --------------------------------------
    // Combine child hashes
    // --------------------------------------

    string leftHash =
        left ? left->hash : "";

    string rightHash =
        right ? right->hash : "";


    string parentHash =
        generateSHA256(
            leftHash + rightHash
        );


    MerkleNode* parent =
        new MerkleNode(parentHash);


    parent->left = left;
    parent->right = right;


    return parent;
}


// ==========================================
// BUILD TREE FROM BLOCKS
// ==========================================

void BinaryMerkleTree::build(
    const vector<Block>& blocks)
{
    // Delete previous tree if build()
    // is called again.

    deleteTree(root);

    root = nullptr;


    if (blocks.empty())
    {
        return;
    }


    vector<string> blockHashes;


    // --------------------------------------
    // Generate hash for every block
    // --------------------------------------

    for (const Block& block : blocks)
    {
        blockHashes.push_back(
            hashBlock(block)
        );
    }


    // --------------------------------------
    // Construct tree
    // --------------------------------------

    root =
        buildTree(
            blockHashes,
            0,
            static_cast<int>(
                blockHashes.size()
            ) - 1
        );
}


// ==========================================
// GET ROOT
// ==========================================

MerkleNode*
BinaryMerkleTree::getRoot() const
{
    return root;
}


// ==========================================
// GET ROOT HASH
// ==========================================

string BinaryMerkleTree::getRootHash() const
{
    if (root == nullptr)
    {
        return "";
    }

    return root->hash;
}