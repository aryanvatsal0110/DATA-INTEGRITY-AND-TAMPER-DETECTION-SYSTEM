
#ifndef TAMPER_REPORT_H
#define TAMPER_REPORT_H

#include <string>
#include <vector>

void generateTamperReport(
    const std::string& trustedRoot,
    const std::string& currentRoot,
    const std::vector<int>& affectedBlocks,
    const std::string& outputFile
);

#endif
