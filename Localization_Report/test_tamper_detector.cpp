
#include "TamperDetector.h"

#include <iostream>
#include <vector>

using namespace std;

int main() {

    cout << "\n========== TAMPER DETECTOR TEST ==========\n";

    TamperDetector detector;

    RootMetadata metadata;

    metadata.originalFileSize = 9000;
    metadata.blockSize = 4096;
    metadata.totalBlocks = 3;

    VerificationResult verification;

    LocalizationResult result;

    int passed = 0;

    // ------------------------------------------
    // TEST 1: Modified block
    // ------------------------------------------

    cout << "\nTEST 1: MODIFIED BLOCK\n";

    verification.integrityMatch = false;
    verification.originalBlockCount = 3;
    verification.currentBlockCount = 3;

    verification.changedBlockIndices = {1};

    bool success = detector.detectChanges(
        verification,
        metadata,
        9000,
        result
    );

    if (success &&
        result.changes.size() == 1 &&
        result.changes[0].changeType == ChangeType::MODIFIED &&
        result.changes[0].startByte == 4096) {

        cout << "PASSED\n";
        cout << "Block: " << result.changes[0].blockIndex << "\n";
        cout << "Start Byte: " << result.changes[0].startByte << "\n";

        passed++;

    }
    else {

        cout << "FAILED\n";

    }


    // ------------------------------------------
    // TEST 2: Added block
    // ------------------------------------------

    cout << "\nTEST 2: ADDED BLOCK\n";

    verification.originalBlockCount = 2;
    verification.currentBlockCount = 3;

    verification.changedBlockIndices = {2};

    success = detector.detectChanges(
        verification,
        metadata,
        10000,
        result
    );

    if (success &&
        result.changes.size() == 1 &&
        result.changes[0].changeType == ChangeType::ADDED &&
        result.changes[0].blockIndex == 2) {

        cout << "PASSED\n";
        passed++;

    }
    else {

        cout << "FAILED\n";

    }


    // ------------------------------------------
    // TEST 3: Removed block
    // ------------------------------------------

    cout << "\nTEST 3: REMOVED BLOCK\n";

    verification.originalBlockCount = 3;
    verification.currentBlockCount = 2;

    verification.changedBlockIndices = {2};

    success = detector.detectChanges(
        verification,
        metadata,
        7000,
        result
    );

    if (success &&
        result.changes.size() == 1 &&
        result.changes[0].changeType == ChangeType::REMOVED &&
        result.changes[0].blockIndex == 2) {

        cout << "PASSED\n";
        passed++;

    }
    else {

        cout << "FAILED\n";

    }


    // ------------------------------------------
    // FINAL RESULT
    // ------------------------------------------

    cout << "\n========== FINAL RESULT ==========\n";

    cout << "Passed: " << passed << "/3\n";

    if (passed == 3) {

        cout << "\nSUCCESS: TamperDetector validated!\n";

    }
    else {

        cout << "\nSome tests failed.\n";

        return 1;
    }

    return 0;
}
