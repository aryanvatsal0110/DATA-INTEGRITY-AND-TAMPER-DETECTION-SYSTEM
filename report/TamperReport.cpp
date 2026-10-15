
#include "TamperReport.h"
#include "../detection/TamperDetector.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void generateTamperReport(
    const std::string& trustedRoot,
    const std::string& currentRoot,
    const std::vector<int>& affectedBlocks,
    const std::string& outputFile)
{
    std::ofstream report(outputFile);

    if (!report)
    {
        std::cerr << "ERROR: Cannot create report: "
                  << outputFile << '\n';
        return;
    }

    const bool tampered =
        detectTampering(trustedRoot, currentRoot);

    report << "DATA INTEGRITY AND TAMPER DETECTION REPORT\n";
    report << "==========================================\n";
    report << "Trusted root: " << trustedRoot << '\n';
    report << "Current root: " << currentRoot << '\n';
    report << "Integrity status: "
           << (tampered ? "MISMATCH DETECTED"
                        : "ROOT MATCHES")
           << '\n';

    report << "Affected blocks:\n";

    if (affectedBlocks.empty())
    {
        report << (tampered
            ? "No individual affected block identified.\n"
            : "None identified.\n");
    }
    else
    {
        for (int id : affectedBlocks)
        {
            if (id == -1)
                report << "Block layout changed; localization "
                          "requires further analysis.\n";
            else
                report << "Block ID: " << id << '\n';
        }
    }

    report.close();

    std::cout << "Tamper report written to: "
              << outputFile << '\n';
}
