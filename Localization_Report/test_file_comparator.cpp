
#include "FileComparator.h"

#include <iostream>
#include <string>

using namespace std;

void printResult(const FileComparisonResult& result) {

    for (const LineChange& change : result.changes) {

        if (change.changeType == LineChangeType::MODIFIED) {

            cout << "\nMODIFIED LINE\n";

            cout << "Original line: "
                 << change.originalLineNumber << "\n";

            cout << "Current line: "
                 << change.currentLineNumber << "\n";

            cout << "Old text: " << change.originalText << "\n";
            cout << "New text: " << change.currentText << "\n";

            cout << "Removed words: ";

            for (const string& word : change.removedWords) {
                cout << word << " ";
            }

            cout << "\nAdded words: ";

            for (const string& word : change.addedWords) {
                cout << word << " ";
            }

            cout << "\n";
        }

        else if (change.changeType == LineChangeType::ADDED) {

            cout << "\nADDED LINE "
                 << change.currentLineNumber << ": "
                 << change.currentText << "\n";

        }

        else if (change.changeType == LineChangeType::REMOVED) {

            cout << "\nREMOVED LINE "
                 << change.originalLineNumber << ": "
                 << change.originalText << "\n";

        }
    }
}

int main() {

    cout << "\n========== FILE COMPARATOR TEST ==========\n";

    FileComparator comparator;

    FileComparisonResult result;

    int passed = 0;
    const int total = 5;

    // ------------------------------------------
    // TEST 1: Word modification
    // ------------------------------------------

    cout << "\nTEST 1: WORD MODIFICATION\n";

    string original1 =
        "TemperD is an integrity system.\n"
        "It detects file modifications.\n"
        "Merkle trees generate hashes.\n";

    string modified1 =
        "TemperD is a security system.\n"
        "It detects file modifications.\n"
        "Merkle trees generate hashes.\n";

    if (comparator.compareText(original1, modified1, result) &&
        result.changes.size() == 1 &&
        result.changes[0].changeType == LineChangeType::MODIFIED &&
        result.changes[0].originalLineNumber == 1 &&
        result.changes[0].currentLineNumber == 1) {

        cout << "PASSED\n";
        printResult(result);
        passed++;

    }
    else {
        cout << "FAILED\n";
    }


    // ------------------------------------------
    // TEST 2: Added line in the middle
    // ------------------------------------------

    cout << "\nTEST 2: LINE INSERTION\n";

    string original2 =
        "Line one\n"
        "Line two\n"
        "Line three\n";

    string modified2 =
        "Line one\n"
        "New inserted line\n"
        "Line two\n"
        "Line three\n";

    if (comparator.compareText(original2, modified2, result) &&
        result.changes.size() == 1 &&
        result.changes[0].changeType == LineChangeType::ADDED &&
        result.changes[0].currentLineNumber == 2) {

        cout << "PASSED\n";
        printResult(result);
        passed++;

    }
    else {
        cout << "FAILED\n";
    }


    // ------------------------------------------
    // TEST 3: Removed line
    // ------------------------------------------

    cout << "\nTEST 3: LINE DELETION\n";

    string original3 =
        "Line one\n"
        "Line two\n"
        "Line three\n";

    string modified3 =
        "Line one\n"
        "Line three\n";

    if (comparator.compareText(original3, modified3, result) &&
        result.changes.size() == 1 &&
        result.changes[0].changeType == LineChangeType::REMOVED &&
        result.changes[0].originalLineNumber == 2) {

        cout << "PASSED\n";
        printResult(result);
        passed++;

    }
    else {
        cout << "FAILED\n";
    }


    // ------------------------------------------
    // TEST 4: Multiple changes
    // ------------------------------------------

    cout << "\nTEST 4: MULTIPLE CHANGES\n";

    string original4 =
        "Alpha\n"
        "Beta\n"
        "Gamma\n";

    string modified4 =
        "Alpha\n"
        "Delta\n"
        "Gamma\n"
        "Omega\n";

    if (comparator.compareText(original4, modified4, result) &&
        result.changes.size() == 2) {

        bool foundModified = false;
        bool foundAdded = false;

        for (const LineChange& change : result.changes) {

            if (change.changeType == LineChangeType::MODIFIED) {
                foundModified = true;
            }

            if (change.changeType == LineChangeType::ADDED) {
                foundAdded = true;
            }
        }

        if (foundModified && foundAdded) {
            cout << "PASSED\n";
            printResult(result);
            passed++;
        }
        else {
            cout << "FAILED\n";
        }

    }
    else {
        cout << "FAILED\n";
    }


    // ------------------------------------------
    // TEST 5: Identical files
    // ------------------------------------------

    cout << "\nTEST 5: IDENTICAL FILES\n";

    string original5 =
        "TemperD\n"
        "Merkle Tree\n"
        "SHA-256\n";

    if (comparator.compareText(original5, original5, result) &&
        result.changes.empty()) {

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

    cout << "Passed: " << passed << "/" << total << "\n";

    if (passed == total) {

        cout << "\nSUCCESS: FileComparator validated!\n";

    }
    else {

        cout << "\nERROR: Some tests failed!\n";

        return 1;
    }

    return 0;
}
