
#include "PerformanceEvaluator.h"

#include <iostream>
#include <iomanip>

void displayPerformance(const PerformanceResult& result)
{
    std::cout << "\n--- Performance Evaluation ---\n";
    std::cout << "Dataset size: "
              << result.dataSizeBytes << " bytes\n";

    std::cout << "Record count: "
              << result.recordCount << '\n';

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Processing time: "
              << result.processingTimeMs << " ms\n";
}
