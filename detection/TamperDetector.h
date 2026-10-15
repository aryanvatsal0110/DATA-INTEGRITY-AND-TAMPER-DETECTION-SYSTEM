
#ifndef TAMPER_DETECTOR_H
#define TAMPER_DETECTOR_H

#include <string>

bool detectTampering(
    const std::string& trustedRoot,
    const std::string& currentRoot
);

#endif
