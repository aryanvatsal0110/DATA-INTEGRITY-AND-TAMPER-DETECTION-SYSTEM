
#include "HashGenerator.h"
#include "HierarchicalMerkleTree.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {

    HashGenerator generator;
    HierarchicalMerkleTree tree;

    int passed = 0;
    int total = 0;

    constexpr size_t GROUP_SIZE = 1000;

    // Generate 5000 valid SHA-256 block hashes
    vector<string> allHashes;

    cout << "Generating test hashes...\n";

    for (size_t i = 0; i < 5000; i++) {

        string data = "Block " + to_string(i);

        string hash;

        if (!generator.generateHash(data, hash)) {
            cerr << "Hash generation failed!\n";
            return 1;
        }

        allHashes.push_back(hash);
    }


    cout << "\n========== HIERARCHICAL MERKLE TESTS ==========\n";


    // TEST 1: One block
    {
        vector<string> hashes(allHashes.begin(), allHashes.begin() + 1);

        bool success = tree.buildTree(hashes, GROUP_SIZE);

        total++;

        if (success &&
            tree.getSubtreeCount() == 1 &&
            tree.getLeafCount() == 1 &&
            tree.getGlobalRootHash().size() == 64) {

            cout << "1 block test: PASSED\n";
            passed++;
        }
        else {
            cout << "1 block test: FAILED\n";
        }
    }


    // TEST 2: Exactly 1000 blocks
    {
        vector<string> hashes(
            allHashes.begin(),
            allHashes.begin() + 1000
        );

        bool success = tree.buildTree(hashes, GROUP_SIZE);

        total++;

        if (success &&
            tree.getSubtreeCount() == 1 &&
            tree.getLeafCount() == 1000) {

            cout << "1000 blocks test: PASSED\n";
            passed++;
        }
        else {
            cout << "1000 blocks test: FAILED\n";
        }
    }


    // TEST 3: 1001 blocks
    {
        vector<string> hashes(
            allHashes.begin(),
            allHashes.begin() + 1001
        );

        bool success = tree.buildTree(hashes, GROUP_SIZE);

        total++;

        if (success &&
            tree.getSubtreeCount() == 2 &&
            tree.getLeafCount() == 1001) {

            cout << "1001 blocks test: PASSED\n";
            passed++;
        }
        else {
            cout << "1001 blocks test: FAILED\n";
        }
    }


    // TEST 4: 5000 blocks
    {
        bool success = tree.buildTree(allHashes, GROUP_SIZE);

        total++;

        if (success &&
            tree.getSubtreeCount() == 5 &&
            tree.getLeafCount() == 5000 &&
            tree.getGlobalRootHash().size() == 64) {

            cout << "5000 blocks test: PASSED\n";
            passed++;
        }
        else {
            cout << "5000 blocks test: FAILED\n";
        }
    }


    // TEST 5: Verify subtree configuration
    {
        bool valid = true;

        if (tree.getBlocksPerSubtree() != GROUP_SIZE) {
            valid = false;
        }

        for (size_t i = 0; i < tree.getSubtreeCount(); i++) {

            const MerkleTree* subtree = tree.getSubtree(i);

            if (subtree == nullptr ||
                tree.getSubtreeRoot(i).size() != 64 ||
                subtree->getRootHash() != tree.getSubtreeRoot(i)) {

                valid = false;
                break;
            }
        }

        total++;

        if (valid) {
            cout << "Subtree configuration test: PASSED\n";
            passed++;
        }
        else {
            cout << "Subtree configuration test: FAILED\n";
        }
    }


    // TEST 6: Deterministic global root
    {
        string firstRoot = tree.getGlobalRootHash();

        bool success = tree.buildTree(allHashes, GROUP_SIZE);

        string secondRoot = tree.getGlobalRootHash();

        total++;

        if (success && firstRoot == secondRoot) {
            cout << "Deterministic root test: PASSED\n";
            passed++;
        }
        else {
            cout << "Deterministic root test: FAILED\n";
        }
    }


    // TEST 7: Empty input
    {
        vector<string> emptyHashes;

        bool success = tree.buildTree(emptyHashes, GROUP_SIZE);

        total++;

        if (!success &&
            tree.getGlobalRootHash().empty() &&
            tree.getSubtreeCount() == 0) {

            cout << "Empty input test: PASSED\n";
            passed++;
        }
        else {
            cout << "Empty input test: FAILED\n";
        }
    }


    // TEST 8: Invalid hash
    {
        vector<string> invalidHashes = {
            "INVALID_HASH"
        };

        bool success = tree.buildTree(invalidHashes, GROUP_SIZE);

        total++;

        if (!success &&
            tree.getGlobalRootHash().empty() &&
            tree.getSubtreeCount() == 0) {

            cout << "Invalid hash test: PASSED\n";
            passed++;
        }
        else {
            cout << "Invalid hash test: FAILED\n";
        }
    }


    // TEST 9: Zero group size
    {
        bool success = tree.buildTree(allHashes, 0);

        total++;

        if (!success &&
            tree.getGlobalRootHash().empty() &&
            tree.getSubtreeCount() == 0) {

            cout << "Zero group size test: PASSED\n";
            passed++;
        }
        else {
            cout << "Zero group size test: FAILED\n";
        }
    }


    // TEST 10: Invalid subtree index
    {
        total++;

        if (tree.getSubtree(0) == nullptr &&
            tree.getSubtreeRoot(0).empty()) {

            cout << "Invalid subtree index test: PASSED\n";
            passed++;
        }
        else {
            cout << "Invalid subtree index test: FAILED\n";
        }
    }


    // Final result
    cout << "\n========== FINAL RESULT ==========\n";

    cout << "Passed: " << passed << "/" << total << "\n";

    if (passed == total) {

        cout << "\nSUCCESS: Hierarchical Merkle Tree validated!\n";

    }
    else {

        cout << "\nERROR: Hierarchical Merkle Tree validation failed!\n";

        return 1;
    }

    return 0;
}
