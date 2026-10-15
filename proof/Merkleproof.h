
#ifndef MERKLE_PROOF_H
#define MERKLE_PROOF_H

#include <string>
#include <vector>

struct ProofStep
{
    std::string siblingHash;

    // true: sibling is on the left
    // false: sibling is on the right
    bool siblingOnLeft;
};

struct MerkleProof
{
    int blockID = -1;
    std::string leafHash;
    std::vector<ProofStep> steps;
};

// Generate a proof for a leaf/block.
// Returns an empty proof with blockID == -1 for invalid input.
MerkleProof generateMerkleProof(
    const std::vector<std::string>& leafHashes,
    int targetIndex
);

// Regenerate the root hash using the proof.
std::string regenerateRoot(const MerkleProof& proof);

#endif
