
#include <iostream>
#include <vector>
#include <string>

#include "hashing/SHA256.h"
#include "proof/Merkleproof.h"
#include "verification/Verifier.h"

int main()
{
    std::vector<std::string> leafHashes = {
        generateSHA256("Block 0 data"),
        generateSHA256("Block 1 data"),
        generateSHA256("Block 2 data"),
        generateSHA256("Block 3 data")
    };

    // Build a trusted root using the same midpoint tree structure.
    std::string leftHash =
        generateSHA256(leafHashes[0] + leafHashes[1]);

    std::string rightHash =
        generateSHA256(leafHashes[2] + leafHashes[3]);

    std::string trustedRoot =
        generateSHA256(leftHash + rightHash);

    // Generate a Merkle proof for block 2.
    MerkleProof proof = generateMerkleProof(leafHashes, 2);

    std::cout << "Trusted root: " << trustedRoot << '\n';
    std::cout << "Proof block ID: " << proof.blockID << '\n';
    std::cout << "Proof steps: " << proof.steps.size() << '\n';

    if (verifyMerkleProof(proof, trustedRoot))
    {
        std::cout << "PASS: Merkle proof verified.\n";
    }
    else
    {
        std::cout << "FAIL: Merkle proof verification failed.\n";
        return 1;
    }

    // Change the leaf hash to simulate a modified proof.
    proof.leafHash = generateSHA256("Modified Block 2 data");

    if (!verifyMerkleProof(proof, trustedRoot))
    {
        std::cout << "PASS: Modified proof rejected.\n";
    }
    else
    {
        std::cout << "FAIL: Modified proof was accepted.\n";
        return 1;
    }

    return 0;
}

#include <iostream>
#include <vector>
#include <string>

#include "hashing/SHA256.h"
#include "proof/Merkleproof.h"
#include "verification/Verifier.h"

int main()
{
    std::vector<std::string> leafHashes = {
        generateSHA256("Block 0 data"),
        generateSHA256("Block 1 data"),
        generateSHA256("Block 2 data"),
        generateSHA256("Block 3 data")
    };

    // Build a trusted root using the same midpoint tree structure.
    std::string leftHash =
        generateSHA256(leafHashes[0] + leafHashes[1]);

    std::string rightHash =
        generateSHA256(leafHashes[2] + leafHashes[3]);

    std::string trustedRoot =
        generateSHA256(leftHash + rightHash);

    // Generate a Merkle proof for block 2.
    MerkleProof proof = generateMerkleProof(leafHashes, 2);

    std::cout << "Trusted root: " << trustedRoot << '\n';
    std::cout << "Proof block ID: " << proof.blockID << '\n';
    std::cout << "Proof steps: " << proof.steps.size() << '\n';

    if (verifyMerkleProof(proof, trustedRoot))
    {
        std::cout << "PASS: Merkle proof verified.\n";
    }
    else
    {
        std::cout << "FAIL: Merkle proof verification failed.\n";
        return 1;
    }

    // Change the leaf hash to simulate a modified proof.
    proof.leafHash = generateSHA256("Modified Block 2 data");

    if (!verifyMerkleProof(proof, trustedRoot))
    {
        std::cout << "PASS: Modified proof rejected.\n";
    }
    else
    {
        std::cout << "FAIL: Modified proof was accepted.\n";
        return 1;
    }

    return 0;
}

