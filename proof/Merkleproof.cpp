#include "MerkleProof.h"

#include "../hashing/SHA256.h"

#include <iostream>

using namespace std;


// ========================================================
// GENERATE MERKLE PROOF
// ========================================================

MerkleProof generateMerkleProof(
    const vector<string>& leafHashes,
    int targetIndex)
{
    MerkleProof proof;

    proof.blockID = targetIndex;

    proof.leafHash =
        leafHashes[targetIndex];


    vector<string> currentLevel =
        leafHashes;

    int currentIndex =
        targetIndex;


    // ----------------------------------------------------
    // Continue until only root remains
    // ----------------------------------------------------

    while (currentLevel.size() > 1)
    {
        vector<string> nextLevel;


        // -----------------------------------------------
        // Find sibling
        // -----------------------------------------------

        int siblingIndex;

        if (currentIndex % 2 == 0)
        {
            // Current node is LEFT child

            siblingIndex =
                currentIndex + 1;

            if (siblingIndex <
                currentLevel.size())
            {
                ProofStep step;

                step.siblingHash =
                    currentLevel[siblingIndex];

                step.siblingOnLeft = false;

                proof.steps.push_back(step);
            }
        }
        else
        {
            // Current node is RIGHT child

            siblingIndex =
                currentIndex - 1;

            ProofStep step;

            step.siblingHash =
                currentLevel[siblingIndex];

            step.siblingOnLeft = true;

            proof.steps.push_back(step);
        }


        // -----------------------------------------------
        // Build next level
        // -----------------------------------------------

        for (int i = 0;
             i < currentLevel.size();
             i += 2)
        {
            string left =
                currentLevel[i];

            string right;


            if (i + 1 <
                currentLevel.size())
            {
                right =
                    currentLevel[i + 1];
            }
            else
            {
                // Duplicate last node
                right = left;
            }


            string parent =
                generateSHA256(
                    left + right
                );

            nextLevel.push_back(parent);
        }


        currentIndex /= 2;

        currentLevel =
            nextLevel;
    }


    return proof;
}


// ========================================================
// REGENERATE ROOT
// ========================================================

string regenerateRoot(
    const MerkleProof& proof)
{
    string currentHash =
        proof.leafHash;


    for (const ProofStep& step :
         proof.steps)
    {
        if (step.siblingOnLeft)
        {
            currentHash =
                generateSHA256(
                    step.siblingHash +
                    currentHash
                );
        }
        else
        {
            currentHash =
                generateSHA256(
                    currentHash +
                    step.siblingHash
                );
        }
    }


    return currentHash;
}