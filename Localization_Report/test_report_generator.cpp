
#include "ReportGenerator.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main() {

    cout << "\n========== REPORT GENERATOR TEST ==========\n";

    fs::create_directories("storage/reports");

    int passed = 0;
    const int total = 4;

    // ------------------------------------------
    // STEP 1: Prepare verification information
    // ------------------------------------------

    RootMetadata metadata;

    metadata.originalFileSize = 9000;
    metadata.blockSize = 4096;
    metadata.totalBlocks = 3;

    VerificationResult verification;

    verification.integrityMatch = false;

    verification.originalBlockCount = 3;
    verification.currentBlockCount = 3;

    verification.originalRoot = string(64, 'a');
    verification.currentRoot = string(64, 'b');

    verification.changedBlockIndices = {1};


    // ------------------------------------------
    // STEP 2: Perform block localization
    // ------------------------------------------

    TamperDetector detector;

    LocalizationResult localization;

    if (!detector.detectChanges(
            verification,
            metadata,
            9000,
            localization
        )) {

        cerr << "Block localization failed!\n";

        return 1;
    }


    // ------------------------------------------
    // STEP 3: Perform text comparison
    // ------------------------------------------

    FileComparator comparator;

    FileComparisonResult comparison;

    string originalText =
        "TemperD is an integrity system.\n"
        "It detects file modifications.\n";

    string modifiedText =
        "TemperD is a security system.\n"
        "It detects file modifications.\n";

    if (!comparator.compareText(
            originalText,
            modifiedText,
            comparison
        )) {

        cerr << "Text comparison failed!\n";

        return 1;
    }


    // ------------------------------------------
    // STEP 4: Prepare report input
    // ------------------------------------------

    ReportInput input;

    input.originalFilePath =
        "storage/original_files/document.txt";

    input.currentFilePath =
        "storage/uploads/modified_document.txt";

    input.reportDirectory =
        "storage/reports";

    input.currentFileSize = modifiedText.size();

    input.verification = verification;

    input.metadata = metadata;

    input.localization = localization;

    input.comparison = comparison;


    // ------------------------------------------
    // STEP 5: Generate JSON report
    // ------------------------------------------

    ReportGenerator generator;

    ReportResult report;

    cout << "\nTEST 1: REPORT GENERATION\n";

    if (generator.generateJSONReport(input, report)) {

        cout << "PASSED\n";

        cout << "Report ID: "
             << report.reportId << "\n";

        cout << "Generated at: "
             << report.generatedAt << "\n";

        cout << "Report path: "
             << report.reportPath << "\n";

        passed++;

    }
    else {

        cout << "FAILED: "
             << report.message << "\n";

        return 1;
    }


    // ------------------------------------------
    // STEP 6: Check file existence
    // ------------------------------------------

    cout << "\nTEST 2: REPORT FILE EXISTENCE\n";

    if (fs::exists(report.reportPath) &&
        fs::file_size(report.reportPath) > 0) {

        cout << "PASSED\n";

        passed++;

    }
    else {

        cout << "FAILED\n";

    }


    // ------------------------------------------
    // STEP 7: Read generated JSON report
    // ------------------------------------------

    cout << "\nTEST 3: JSON CONTENT VALIDATION\n";

    ifstream file(report.reportPath, ios::binary);

    stringstream buffer;

    buffer << file.rdbuf();

    string json = buffer.str();

    bool validContent =
        json.find("\"status\": \"TAMPERED\"") != string::npos &&
        json.find("\"change_type\": \"MODIFIED\"") != string::npos &&
        json.find("\"original_line\": 1") != string::npos &&
        json.find("\"current_line\": 1") != string::npos &&
        json.find("\"integrity\"") != string::npos &&
        json.find("\"text_comparison\"") != string::npos;

    if (validContent) {

        cout << "PASSED\n";

        passed++;

    }
    else {

        cout << "FAILED\n";

        cout << json << "\n";

    }


    // ------------------------------------------
    // STEP 8: Validate JSON escaping
    // ------------------------------------------

    cout << "\nTEST 4: SPECIAL CHARACTER ESCAPING\n";

    ReportInput escapeInput = input;

    escapeInput.currentFilePath =
        "storage/uploads/modified\"document.txt";

    ReportResult escapeReport;

    if (generator.generateJSONReport(
            escapeInput,
            escapeReport
        )) {

        ifstream escapeFile(
            escapeReport.reportPath,
            ios::binary
        );

        stringstream escapeBuffer;

        escapeBuffer << escapeFile.rdbuf();

        string escapeJSON = escapeBuffer.str();

        if (escapeJSON.find(
                "modified\\\"document.txt"
            ) != string::npos) {

            cout << "PASSED\n";

            passed++;

        }
        else {

            cout << "FAILED\n";

        }
    }
    else {

        cout << "FAILED: "
             << escapeReport.message << "\n";

    }


    // ------------------------------------------
    // FINAL RESULT
    // ------------------------------------------

    cout << "\n========== FINAL RESULT ==========\n";

    cout << "Passed: " << passed << "/" << total << "\n";

    if (passed == total) {

        cout << "\nSUCCESS: ReportGenerator validated!\n";

    }
    else {

        cout << "\nERROR: Some tests failed!\n";

        return 1;
    }

    return 0;
}
