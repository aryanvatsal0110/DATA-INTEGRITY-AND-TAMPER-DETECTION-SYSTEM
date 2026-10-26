
#include "Verifier.h"
#include "../proof/Merkleproof.h"

#include <string>

bool verifyMerkleProof(
    const MerkleProof& proof,
    const std::string& trustedRoot
)
{
    if (trustedRoot.empty() ||
        proof.blockID < 0 ||
        proof.leafHash.empty())
    {
        return false;
    }

    const std::string regeneratedRoot = regenerateRoot(proof);

    return !regeneratedRoot.empty() &&
           regeneratedRoot == trustedRoot;
}
