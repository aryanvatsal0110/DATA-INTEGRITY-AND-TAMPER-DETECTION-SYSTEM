#include "HierarchicalMerkleTree.h"

#include "../hashing/SHA256.h"

using namespace std;


// ==========================================
// CONSTRUCTOR
// ==========================================

HierarchicalMerkleTree::HierarchicalMerkleTree(
    int subtreeSize)
{
    root = nullptr;

    blocksPerSubtree = subtreeSize;

    if (blocksPerSubtree < 2)
    {
        blocksPerSubtree = 2;
    }
}


// ==========================================
// DESTRUCTOR
// ==========================================

HierarchicalMerkleTree::~HierarchicalMerkleTree()
{
    deleteTree(root);
}


// ==========================================
// DELETE TREE
// ==========================================

void HierarchicalMerkleTree::deleteTree(
    MerkleNode* node)
{
    if (node == nullptr)
        return;

    deleteTree(node->left);
    deleteTree(node->right);

    delete node;
}


// ==========================================
// HASH ONE BLOCK
// ==========================================

string HierarchicalMerkleTree::hashBlock(
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
// BUILD ONE SUBTREE
// ==========================================

MerkleNode*
HierarchicalMerkleTree::buildSubtree(
    const vector<Block>& blocks,
    int start,
    int end)
{
    if (start > end)
    {
        return nullptr;
    }


    // --------------------------------------
    // One block = leaf
    // --------------------------------------

    if (start == end)
    {
        string blockHash =
            hashBlock(blocks[start]);

        return new MerkleNode(blockHash);
    }


    // --------------------------------------
    // Divide subtree
    // --------------------------------------

    int middle =
        start + (end - start) / 2;


    MerkleNode* left =
        buildSubtree(
            blocks,
            start,
            middle
        );


    MerkleNode* right =
        buildSubtree(
            blocks,
            middle + 1,
            end
        );


    string leftHash =
        left ? left->hash : "";

    string rightHash =
        right ? right->hash : "";


    // --------------------------------------
    // Create subtree root
    // --------------------------------------

    string subtreeHash =
        generateSHA256(
            leftHash + rightHash
        );


    MerkleNode* node =
        new MerkleNode(subtreeHash);


    node->left = left;
    node->right = right;


    return node;
}


// ==========================================
// BUILD TOP LEVEL TREE
// ==========================================

MerkleNode*
HierarchicalMerkleTree::buildTopLevelTree(
    const vector<MerkleNode*>& subtreeRoots,
    int start,
    int end)
{
    if (start > end)
    {
        return nullptr;
    }


    // --------------------------------------
    // One subtree root
    // --------------------------------------

    if (start == end)
    {
        return new MerkleNode(
            subtreeRoots[start]->hash
        );
    }


    int middle =
        start + (end - start) / 2;


    MerkleNode* left =
        buildTopLevelTree(
            subtreeRoots,
            start,
            middle
        );


    MerkleNode* right =
        buildTopLevelTree(
            subtreeRoots,
            middle + 1,
            end
        );


    string leftHash =
        left ? left->hash : "";

    string rightHash =
        right ? right->hash : "";


    string globalHash =
        generateSHA256(
            leftHash + rightHash
        );


    MerkleNode* parent =
        new MerkleNode(globalHash);


    parent->left = left;
    parent->right = right;


    return parent;
}


// ==========================================
// BUILD HIERARCHICAL TREE
// ==========================================

void HierarchicalMerkleTree::build(
    const vector<Block>& blocks)
{
    deleteTree(root);

    root = nullptr;


    if (blocks.empty())
    {
        return;
    }


    vector<MerkleNode*> subtreeRoots;


    // ======================================
    // STEP 1
    // Divide blocks into groups
    // ======================================

    for (
        int start = 0;
        start < static_cast<int>(blocks.size());
        start += blocksPerSubtree
    )
    {
        int end =
            start + blocksPerSubtree - 1;


        if (end >= static_cast<int>(blocks.size()))
        {
            end =
                static_cast<int>(blocks.size()) - 1;
        }


        // Build one subtree

        MerkleNode* subtree =
            buildSubtree(
                blocks,
                start,
                end
            );


        subtreeRoots.push_back(subtree);
    }


    // ======================================
    // STEP 2
    // Combine subtree roots
    // ======================================

    if (subtreeRoots.size() == 1)
    {
        root =
            new MerkleNode(
                subtreeRoots[0]->hash
            );

        deleteTree(subtreeRoots[0]);

        return;
    }


    // ======================================
    // STEP 3
    // Build global root
    // ======================================

    root =
        buildTopLevelTree(
            subtreeRoots,
            0,
            static_cast<int>(
                subtreeRoots.size()
            ) - 1
        );


    // ======================================
    // STEP 4
    // Delete temporary subtree trees
    // ======================================

    for (MerkleNode* subtree :
         subtreeRoots)
    {
        deleteTree(subtree);
    }
}


// ==========================================
// GET ROOT
// ==========================================

MerkleNode*
HierarchicalMerkleTree::getRoot() const
{
    return root;
}


// ==========================================
// GET ROOT HASH
// ==========================================

string
HierarchicalMerkleTree::getRootHash() const
{
    if (root == nullptr)
    {
        return "";
    }

    return root->hash;
}