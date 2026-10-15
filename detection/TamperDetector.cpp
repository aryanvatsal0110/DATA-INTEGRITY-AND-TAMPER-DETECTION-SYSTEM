
#include "TamperDetector.h"

bool detectTampering(
    const std::string& trustedRoot,
    const std::string& currentRoot)
{
    // Missing roots cannot establish integrity.
    if (trustedRoot.empty() || currentRoot.empty())
        return true;

    return trustedRoot != currentRoot;
}
