
#ifndef TAMPER_LOCALIZER_H
#define TAMPER_LOCALIZER_H

#include <string>
#include <vector>

#include "../tree/Block.h"

std::vector<int> locateTamperedBlocks(
    const std::vector<Block>& blocks,
    const std::vector<std::string>& trustedLeafHashes
);

#endif
