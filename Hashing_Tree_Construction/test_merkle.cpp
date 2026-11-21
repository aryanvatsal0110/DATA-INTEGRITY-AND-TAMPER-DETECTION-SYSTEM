
#include "HashGenerator.h"
#include "MerkleTree.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {

    HashGenerator generator;
    MerkleTree tree;

    int passed = 0;
    int total = 0;

    // Generate test block hashes
    vector<string> testData = {
        "Block A",
        "Block B",
        "Block C",
        "Block D",
        "Block E"
    };

    vector<string> allHashes;

    for (const string& data : testData) {

        string hash;

        if (!generator.generateHash(data, hash)) {
            cerr << "Hash generation failed!\n";
            return 1;
        }

        allHashes.push_back(hash);
    }

    cout << "\n========== MERKLE TREE TESTS ==========\n";


    // Test trees containing 1 to 5 leaves
    for (size_t count = 1; count <= 5; count++) {

        vector<string> hashes(
            allHashes.begin(),
            allHashes.begin() + count
        );

        bool success = tree.buildTree(hashes);

        total++;

        if (success &&
            tree.getLeafCount() == count &&
            tree.getRootHash().size() == 64 &&
            tree.getRoot() != nullptr) {

            cout << count << " leaf tree: PASSED\n";
            passed++;

        }
        else {
            cout << count << " leaf tree: FAILED\n";
        }
    }


    // Test deterministic root generation
    vector<string> hashes(
        allHashes.begin(),
        allHashes.begin() + 4
    );

    tree.buildTree(hashes);

    string firstRoot = tree.getRootHash();

    tree.buildTree(hashes);

    string secondRoot = tree.getRootHash();

    total++;

    if (firstRoot == secondRoot) {

        cout << "Deterministic root test: PASSED\n";
        passed++;

    }
    else {
        cout << "Deterministic root test: FAILED\n";
    }


    // Test whether modified input changes the root
    vector<string> modifiedHashes = hashes;

    string modifiedHash;

    generator.generateHash("Modified Block B", modifiedHash);

    modifiedHashes[1] = modifiedHash;

    tree.buildTree(hashes);

    string originalRoot = tree.getRootHash();

    tree.buildTree(modifiedHashes);

    string modifiedRoot = tree.getRootHash();

    total++;

    if (originalRoot != modifiedRoot) {

        cout << "Tamper detection test: PASSED\n";
        passed++;

    }
    else {
        cout << "Tamper detection test: FAILED\n";
    }


    // Test empty input rejection
    vector<string> emptyHashes;

    total++;

    if (!tree.buildTree(emptyHashes) &&
        tree.getRoot() == nullptr &&
        tree.getRootHash().empty()) {

        cout << "Empty input test: PASSED\n";
        passed++;

    }
    else {
        cout << "Empty input test: FAILED\n";
    }


    // Test invalid hash rejection
    vector<string> invalidHashes = {
        "INVALID_HASH"
    };

    total++;

    if (!tree.buildTree(invalidHashes) &&
        tree.getRoot() == nullptr) {

        cout << "Invalid hash test: PASSED\n";
        passed++;

    }
    else {
        cout << "Invalid hash test: FAILED\n";
    }


    // Display final Merkle Root
    tree.buildTree(hashes);

    cout << "\n========== MERKLE ROOT ==========\n";
    cout << tree.getRootHash() << "\n";


    // Final result
    cout << "\n========== FINAL RESULT ==========\n";

    cout << "Passed: " << passed << "/" << total << "\n";

    if (passed == total) {

        cout << "\nSUCCESS: Merkle Tree validated!\n";

    }
    else {

        cout << "\nERROR: Merkle Tree validation failed!\n";

        return 1;
    }

    return 0;
}
