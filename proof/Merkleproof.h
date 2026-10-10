#ifndef MERKLE_PROOF_H
#define MERKLE_PROOF_H

#include <string>
#include <vector>

struct ProofStep
{
    std::string siblingHash;

    // true  -> sibling is on left
    // false -> sibling is on right
    bool siblingOnLeft;
};

struct MerkleProof
{
    int blockID;

    std::string leafHash;

    std::vector<ProofStep> steps;
};


// Generate proof for a leaf/block
MerkleProof generateMerkleProof(
    const std::vector<std::string>& leafHashes,
    int targetIndex
);


// Regenerate root using the proof
std::string regenerateRoot(
    const MerkleProof& proof
);


#endif