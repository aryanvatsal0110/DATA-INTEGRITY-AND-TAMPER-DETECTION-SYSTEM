
#include "RootManager.h"
#include "BaselineStorage.h"
#include "IntegrityVerifier.h"

#include "FileHandler.h"
#include "Partitioner.h"
#include "HashGenerator.h"
#include "MerkleTree.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

using namespace std;
namespace fs = std::filesystem;


bool writeFile(const string& path, const string& data) {

    ofstream file(path, ios::binary | ios::trunc);

    if (!file.is_open()) {
        return false;
    }

    file.write(data.data(), data.size());

    return file.good();
}


int main() {

    cout << "\n========== TEMPERD VERIFICATION TEST ==========\n";

    fs::create_directories("storage/uploads");
    fs::create_directories("storage/baselines");

    const string originalPath =
        "storage/uploads/verification_original.txt";

    const string modifiedPath =
        "storage/uploads/verification_modified.txt";

    const string appendedPath =
        "storage/uploads/verification_appended.txt";

    const string baselinePath =
        "storage/baselines/test_baseline.tdb";


    // STEP 1: Create original file
    string originalData;

    for (int i = 0; i < 200; i++) {
        originalData += "TemperD integrity verification test.\n";
    }

    if (!writeFile(originalPath, originalData)) {
        cerr << "Failed to create original file!\n";
        return 1;
    }

    cout << "\nOriginal file created successfully.\n";


    // STEP 2: Read original file
    FileHandler handler;

    string fileData;

    if (!handler.readFile(originalPath, fileData)) {
        cerr << "File reading failed!\n";
        return 1;
    }


    // STEP 3: Partition original file
    Partitioner partitioner;

    vector<Block> blocks;

    PartitionConfig config;

    partitioner.partitionFile(
        fileData,
        blocks,
        config
    );


    // STEP 4: Generate original block hashes
    HashGenerator generator;

    vector<string> blockHashes;

    if (!generator.generateBlockHashes(
            blocks,
            blockHashes
        )) {

        cerr << "Hash generation failed!\n";
        return 1;
    }


    // STEP 5: Construct original Merkle Tree
    MerkleTree tree;

    if (!tree.buildTree(blockHashes)) {
        cerr << "Merkle Tree construction failed!\n";
        return 1;
    }

    string originalRoot = tree.getRootHash();


    // STEP 6: Create original root metadata
    RootManager rootManager;

    if (!rootManager.setRootMetadata(
            originalRoot,
            fileData.size(),
            config.blockSize,
            config.totalBlocks,
            TreeType::STANDARD,
            0
        )) {

        cerr << "Root metadata creation failed!\n";
        return 1;
    }


    // STEP 7: Save original baseline
    BaselineStorage storage;

    if (!storage.saveBaseline(
            baselinePath,
            *rootManager.getMetadata(),
            blockHashes
        )) {

        cerr << "Baseline storage failed!\n";
        return 1;
    }

    cout << "\nOriginal baseline saved successfully.\n";

    cout << "Original Merkle Root: "
         << originalRoot << "\n";

    cout << "Original Blocks: "
         << config.totalBlocks << "\n";


    // STEP 8: Initialize verifier
    IntegrityVerifier verifier;

    VerificationResult result;

    int passed = 0;
    int total = 3;


    // TEST 1: Verify unchanged file
    cout << "\n========== TEST 1 ==========\n";

    if (verifier.verifyFile(
            originalPath,
            baselinePath,
            result
        )) {

        cout << result.message << "\n";

        if (result.integrityMatch &&
            result.changedBlockIndices.empty()) {

            cout << "UNCHANGED FILE TEST: PASSED\n";
            passed++;

        }
        else {
            cout << "UNCHANGED FILE TEST: FAILED\n";
        }

    }
    else {
        cout << "Verification error: "
             << result.message << "\n";
        cout << "UNCHANGED FILE TEST: FAILED\n";
    }


    // TEST 2: Modify existing content
    cout << "\n========== TEST 2 ==========\n";

    string modifiedData = originalData;

    modifiedData[100] = 'X';

    if (!writeFile(modifiedPath, modifiedData)) {
        cerr << "Modified file creation failed!\n";
        return 1;
    }

    if (verifier.verifyFile(
            modifiedPath,
            baselinePath,
            result
        )) {

        cout << result.message << "\n";

        cout << "Changed Blocks: ";

        for (size_t index : result.changedBlockIndices) {
            cout << index << " ";
        }

        cout << "\n";

        if (!result.integrityMatch &&
            !result.changedBlockIndices.empty()) {

            cout << "MODIFIED FILE TEST: PASSED\n";
            passed++;

        }
        else {
            cout << "MODIFIED FILE TEST: FAILED\n";
        }

    }
    else {
        cout << "Verification error: "
             << result.message << "\n";
        cout << "MODIFIED FILE TEST: FAILED\n";
    }


    // TEST 3: Append new content
    cout << "\n========== TEST 3 ==========\n";

    string appendedData = originalData;

    appendedData += string(1000, 'A');

    if (!writeFile(appendedPath, appendedData)) {
        cerr << "Appended file creation failed!\n";
        return 1;
    }

    if (verifier.verifyFile(
            appendedPath,
            baselinePath,
            result
        )) {

        cout << result.message << "\n";

        cout << "Original Blocks: "
             << result.originalBlockCount << "\n";

        cout << "Current Blocks: "
             << result.currentBlockCount << "\n";

        cout << "Changed/Added Blocks: ";

        for (size_t index : result.changedBlockIndices) {
            cout << index << " ";
        }

        cout << "\n";

        if (!result.integrityMatch &&
            result.currentBlockCount > result.originalBlockCount &&
            !result.changedBlockIndices.empty()) {

            cout << "APPENDED CONTENT TEST: PASSED\n";
            passed++;

        }
        else {
            cout << "APPENDED CONTENT TEST: FAILED\n";
        }

    }
    else {
        cout << "Verification error: "
             << result.message << "\n";
        cout << "APPENDED CONTENT TEST: FAILED\n";
    }


    // FINAL RESULT
    cout << "\n========== FINAL RESULT ==========\n";

    cout << "Passed: " << passed << "/" << total << "\n";

    if (passed == total) {

        cout << "\nSUCCESS: Integrity Verification validated!\n";

    }
    else {

        cout << "\nERROR: Some verification tests failed!\n";

        return 1;
    }

    return 0;
}
