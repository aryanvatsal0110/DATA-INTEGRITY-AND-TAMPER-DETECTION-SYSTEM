
#include "InputHandler.h"

#include <fstream>
#include <iostream>
#include <iterator>

// Existing text-based input function
std::vector<std::string> readDataset(const std::string& filename)
{
    std::vector<std::string> records;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open dataset: "
                  << filename << std::endl;
        return records;
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            records.push_back(line);
        }
    }

    return records;
}

// New binary-safe input function
std::vector<std::uint8_t> readFileBytes(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file: "
                  << filename << std::endl;
        return {};
    }

    return std::vector<std::uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}
