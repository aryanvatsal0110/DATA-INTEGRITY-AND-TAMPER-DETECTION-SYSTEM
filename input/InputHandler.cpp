#include "InputHandler.h"
#include <fstream>
#include <iostream>

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

    file.close();

    return records;
}