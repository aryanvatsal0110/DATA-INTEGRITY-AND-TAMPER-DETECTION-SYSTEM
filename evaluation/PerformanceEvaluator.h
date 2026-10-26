
#ifndef PERFORMANCE_EVALUATOR_H
#define PERFORMANCE_EVALUATOR_H

#include <string>

struct PerformanceResult
{
    double processingTimeMs;
    long long dataSizeBytes;
    int recordCount;
};

void displayPerformance(const PerformanceResult& result);

#endif
