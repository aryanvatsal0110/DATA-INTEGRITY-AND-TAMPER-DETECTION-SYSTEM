
#include "Merkleproof.h"
#include "../hashing/SHA256.h"

#include <string>
#include <vector>

using namespace std;

// Calculate the root hash using the SAME midpoint-splitting
// structure as BinaryMerkleTree::buildTree().
static string calculateSubtreeHash(
    const vector<string>& leafHashes,
    int start,
    int end)
{
    if (start == end)
        return leafHashes[start];

    int middle = start + (end - start) / 2;

    string leftHash =
        calculateSubtreeHash(leafHashes, start, middle);

    string rightHash =
        calculateSubtreeHash(leafHashes, middle + 1, end);

    return generateSHA256(leftHash + rightHash);
}

// Generate a proof by following the same recursive tree structure.
static bool buildProofSteps(
    const vector<string>& leafHashes,
    int start,
    int end,
    int targetIndex,
    vector<ProofStep>& steps)
{
    if (start == end)
        return start == targetIndex;

    int middle = start + (end - start) / 2;

    if (targetIndex <= middle)
    {
        // Target is in the left subtree.
        if (!buildProofSteps(
                leafHashes, start, middle,
                targetIndex, steps))
            return false;

        ProofStep step;
        step.siblingHash =
            calculateSubtreeHash(
                leafHashes, middle + 1, end);

        step.siblingOnLeft = false;
        steps.push_back(step);
    }
    else
    {
        // Target is in the right subtree.
        if (!buildProofSteps(
                leafHashes, middle + 1, end,
                targetIndex, steps))
            return false;

        ProofStep step;
        step.siblingHash =
            calculateSubtreeHash(
                leafHashes, start, middle);

        step.siblingOnLeft = true;
        steps.push_back(step);
    }

    return true;
}

MerkleProof generateMerkleProof(
    const vector<string>& leafHashes,
    int targetIndex)
{
    MerkleProof proof;
    proof.blockID = -1;

    if (leafHashes.empty() ||
        targetIndex < 0 ||
        targetIndex >= static_cast<int>(leafHashes.size()))
    {
        return proof;
    }

    proof.blockID = targetIndex;
    proof.leafHash = leafHashes[targetIndex];

    if (!buildProofSteps(
            leafHashes,
            0,
            static_cast<int>(leafHashes.size()) - 1,
            targetIndex,
            proof.steps))
    {
        proof.blockID = -1;
        proof.steps.clear();
    }

    return proof;
}

string regenerateRoot(const MerkleProof& proof)
{
    if (proof.blockID < 0 || proof.leafHash.empty())
        return "";

    string currentHash = proof.leafHash;

    for (const ProofStep& step : proof.steps)
    {
        if (step.siblingHash.empty())
            return "";

        if (step.siblingOnLeft)
        {
            currentHash =
                generateSHA256(step.siblingHash + currentHash);
        }
        else
        {
            currentHash =
                generateSHA256(currentHash + step.siblingHash);
        }
    }

    return currentHash;
}
