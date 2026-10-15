
#ifndef VERIFIER_H
#define VERIFIER_H

#include <string>
#include "../proof/Merkleproof.h"

// Verify a Merkle proof against the trusted root hash.
bool verifyMerkleProof(
    const MerkleProof& proof,
    const std::string& trustedRoot
);

#endif
