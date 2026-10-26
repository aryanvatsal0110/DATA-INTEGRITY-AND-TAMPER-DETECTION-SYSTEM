
#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <string>
#include <vector>
#include <cstdint>

// Existing text-based input function
std::vector<std::string> readDataset(const std::string& filename);

// New binary-safe input function
std::vector<std::uint8_t> readFileBytes(const std::string& filename);

#endif
