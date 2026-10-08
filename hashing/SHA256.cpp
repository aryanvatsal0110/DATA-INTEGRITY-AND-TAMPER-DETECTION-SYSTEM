
#include "SHA256.h"

#include <openssl/sha.h>

#include <iomanip>
#include <sstream>

std::string generateSHA256(const std::string& data)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(
        reinterpret_cast<const unsigned char*>(data.c_str()),
        data.size(),
        hash
    );

    std::stringstream result;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        result << std::hex
               << std::setw(2)
               << std::setfill('0')
               << static_cast<int>(hash[i]);
    }

    return result.str();
}
