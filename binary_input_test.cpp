
#include "input/InputHandler.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <cstdint>
#include <vector>

int main()
{
    const std::string path = "data/sample.txt";

    auto actual = readFileBytes(path);

    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "FAIL: Could not open reference file\n";
        return 1;
    }

    std::vector<std::uint8_t> expected;
    char byte;

    while (file.get(byte))
    {
        expected.push_back(static_cast<std::uint8_t>(
            static_cast<unsigned char>(byte)
        ));
    }

    if (actual.size() != expected.size())
    {
        std::cout << "FAIL: Byte counts differ\n";
        return 1;
    }

    for (std::size_t i = 0; i < actual.size(); ++i)
    {
        if (actual[i] != expected[i])
        {
            std::cout << "FAIL: Byte mismatch at index "
                      << i << '\n';
            return 1;
        }
    }

    std::cout << "PASS: Every byte matches\n";
    std::cout << "Total bytes verified: "
              << actual.size() << '\n';

    return 0;
}
