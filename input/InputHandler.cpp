#include "InputHandler.h"
#include <fstream>
#include <iostream>
// Function to read records from a dataset file.
// filename specifies the name or path of the file.
// Returns a vector containing all non-empty lines.

std::vector<std::string> readDataset(const std::string& filename)
{
    // Create an empty vector to store the dataset records.
    std::vector<std::string> records;
    
    // Open the specified file in read mode.

    std::ifstream file(filename);

      // Check whether the file was opened successfully.

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
