
#include "PerformanceEvaluator.h"

#include <iostream>
#include <iomanip>
// Displays the performance evaluation results.
// Shows dataset size, record count, and processing time.
// Formats processing time up to three decimal places.
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
