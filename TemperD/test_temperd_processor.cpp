
#include "TemperDProcessor.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

using namespace std;
namespace fs = std::filesystem;

int main() {

    cout << "\n========== TEMPERD INTEGRATION TEST ==========\n";

    fs::create_directories("storage/uploads");

    const string originalPath =
        "storage/uploads/integration_original.txt";

    const string modifiedPath =
        "storage/uploads/integration_modified.txt";

    const string storageRoot = "storage";

    // Create test file
    {
        ofstream file(originalPath);

        if (!file.is_open()) {
            cerr << "Cannot create original file.\n";
            return 1;
        }

        for (int i = 0; i < 200; i++) {
            file << "TemperD verifies file integrity.\n";
        }
    }

    TemperDProcessor processor;

    RegistrationResult registration;

    int passed = 0;

    // ------------------------------------------
    // TEST 1: Registration
    // ------------------------------------------

    cout << "\nTEST 1: FILE REGISTRATION\n";

    if (!processor.registerFile(
            originalPath,
            storageRoot,
            registration
        )) {

        cout << "FAILED: "
             << registration.message << "\n";

        return 1;
    }

    cout << "PASSED\n";

    cout << "File ID: "
         << registration.fileId << "\n";

    cout << "Merkle Root: "
         << registration.merkleRoot << "\n";

    cout << "Blocks: "
         << registration.totalBlocks << "\n";

    passed++;


    // ------------------------------------------
    // TEST 2: Unchanged file verification
    // ------------------------------------------

    cout << "\nTEST 2: UNCHANGED FILE VERIFICATION\n";

    TemperDVerificationResult verification;

    if (!processor.verifyRegisteredFile(
            originalPath,
            registration.fileId,
            storageRoot,
            verification
        )) {

        cout << "FAILED: "
             << verification.message << "\n";

        return 1;
    }

    if (verification.verification.integrityMatch &&
        verification.report.success) {

        cout << "PASSED\n";

        passed++;

    }
    else {

        cout << "FAILED\n";

        return 1;
    }


    // ------------------------------------------
    // TEST 3: Modified file verification
    // ------------------------------------------

    cout << "\nTEST 3: MODIFIED FILE VERIFICATION\n";

    string modifiedData;

    {
        ifstream input(originalPath);

        modifiedData.assign(
            istreambuf_iterator<char>(input),
            istreambuf_iterator<char>()
        );
    }

    if (modifiedData.size() > 100) {
        modifiedData[100] = 'X';
    }

    {
        ofstream output(
            modifiedPath,
            ios::binary | ios::trunc
        );

        output.write(
            modifiedData.data(),
            static_cast<streamsize>(modifiedData.size())
        );

        if (!output.good()) {
            cerr << "Cannot write modified file.\n";
            return 1;
        }
    }

    if (!processor.verifyRegisteredFile(
            modifiedPath,
            registration.fileId,
            storageRoot,
            verification
        )) {

        cout << "FAILED: "
             << verification.message << "\n";

        return 1;
    }

    if (!verification.verification.integrityMatch &&
        !verification.localization.changes.empty() &&
        verification.comparison.success &&
        verification.report.success) {

        cout << "PASSED\n";

        cout << "Changed blocks: "
             << verification.localization.changes.size()
             << "\n";

        cout << "Text changes: "
             << verification.comparison.changes.size()
             << "\n";

        cout << "Report: "
             << verification.report.reportPath
             << "\n";

        passed++;

    }
    else {

        cout << "FAILED\n";

        return 1;
    }


    // ------------------------------------------
    // FINAL RESULT
    // ------------------------------------------

    cout << "\n========== FINAL RESULT ==========\n";

    cout << "Passed: " << passed << "/3\n";

    if (passed == 3) {
        cout << "\nSUCCESS: TemperD integration validated!\n";
    }

    return 0;
}
