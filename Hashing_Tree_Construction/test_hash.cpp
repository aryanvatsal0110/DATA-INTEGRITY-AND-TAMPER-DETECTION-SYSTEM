
#include "HashGenerator.h"

#include <iostream>
#include <vector>
#include <string>
#include <utility>

using namespace std;

int main() {

    HashGenerator generator;

    int passed = 0;
    int total = 0;

    // Standard SHA-256 test vectors
    vector<pair<string, string>> testCases = {

        {
            "",
            "e3b0c44298fc1c149afbf4c8996fb924"
            "27ae41e4649b934ca495991b7852b855"
        },

        {
            "abc",
            "ba7816bf8f01cfea414140de5dae2223"
            "b00361a396177a9cb410ff61f20015ad"
        },

        {
            "The quick brown fox jumps over the lazy dog",
            "d7a8fbb307d7809469ca9abcb0082e4f"
            "8d5651e46d3cdb762d02d0bf37c9e592"
        },

        {
            string(55, 'a'),
            "9f4390f8d30c2dd92ec9f095b65e2b9a"
            "e9b0a925a5258e241c9f1e910f734318"
        },

        {
            string(56, 'a'),
            "b35439a4ac6f0948b6d6f9e3c6af0f5f"
            "590ce20f1bde7090ef7970686ec6738a"
        },

        {
            string(63, 'a'),
            "7d3e74a05d7db15bce4ad9ec0658ea98"
            "e3f06eeecf16b4c6fff2da457ddc2f34"
        },

        {
            string(64, 'a'),
            "ffe054fe7ae0cb6dc65c3af9b61d5209"
            "f439851db43d0ba5997337df154668eb"
        },

        {
            string(65, 'a'),
            "635361c48bb9eab14198e76ea8ab7f1a"
            "41685d6ad62aa9146d301d4f17eb0ae0"
        }
    };


    cout << "\n========== SHA-256 TESTS ==========\n";

    // Test individual hashes
    for (size_t i = 0; i < testCases.size(); i++) {

        string actualHash;

        bool success = generator.generateHash(
            testCases[i].first,
            actualHash
        );

        total++;

        if (success && actualHash == testCases[i].second) {

            cout << "Test " << i + 1 << ": PASSED\n";
            passed++;

        }
        else {

            cout << "Test " << i + 1 << ": FAILED\n";

            cout << "Expected: "
                 << testCases[i].second << "\n";

            cout << "Actual: "
                 << actualHash << "\n";
        }
    }


    // Test binary data containing null bytes
    string binaryData("a\0b", 3);

    string binaryHash;

    total++;

    if (generator.generateHash(binaryData, binaryHash) &&
        binaryHash ==
        "59b271ae1bbcb1d31d41929817f4b16f"
        "b439eb4f31520b5ad1d5ce98920a7138") {

        cout << "Binary data test: PASSED\n";
        passed++;

    }
    else {
        cout << "Binary data test: FAILED\n";
    }


    // Test hashing multiple Block objects
    vector<Block> blocks;

    blocks.emplace_back(0, 0, "abc");
    blocks.emplace_back(1, 3, string("a\0b", 3));

    vector<string> blockHashes;

    total++;

    if (generator.generateBlockHashes(blocks, blockHashes) &&
        blockHashes.size() == 2 &&
        blockHashes[0] == testCases[1].second &&
        blockHashes[1] == binaryHash) {

        cout << "Block hashing test: PASSED\n";
        passed++;

    }
    else {
        cout << "Block hashing test: FAILED\n";
    }


    // Final result
    cout << "\n========== FINAL RESULT ==========\n";

    cout << "Passed: " << passed << "/" << total << "\n";

    if (passed == total) {
        cout << "\nSUCCESS: SHA-256 implementation validated!\n";
    }
    else {
        cout << "\nERROR: SHA-256 validation failed!\n";
        return 1;
    }

    return 0;
}
