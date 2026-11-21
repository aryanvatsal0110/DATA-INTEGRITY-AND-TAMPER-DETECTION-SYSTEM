
#include "HashGenerator.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <utility>

namespace {

    // SHA-256 constants
    constexpr std::array<uint32_t, 64> K = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    // Initial hash values
    constexpr std::array<uint32_t, 8> INITIAL_HASH = {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
    };

    // here we are doing Right rotation
    uint32_t rotateRight(uint32_t value, unsigned int bits) {
        return (value >> bits) | (value << (32 - bits));
    }

    // SHA-256 logical functions
    uint32_t choose(uint32_t x, uint32_t y, uint32_t z) {
        return (x & y) ^ (~x & z);
    }

    uint32_t majority(uint32_t x, uint32_t y, uint32_t z) {
        return (x & y) ^ (x & z) ^ (y & z);
    }

    uint32_t sigma0(uint32_t x) {
        return rotateRight(x, 2) ^
               rotateRight(x, 13) ^
               rotateRight(x, 22);
    }

    uint32_t sigma1(uint32_t x) {
        return rotateRight(x, 6) ^
               rotateRight(x, 11) ^
               rotateRight(x, 25);
    }

    uint32_t gamma0(uint32_t x) {
        return rotateRight(x, 7) ^
               rotateRight(x, 18) ^
               (x >> 3);
    }

    uint32_t gamma1(uint32_t x) {
        return rotateRight(x, 17) ^
               rotateRight(x, 19) ^
               (x >> 10);
    }

    // Process one 512-bit message block
    void processBlock(
        const unsigned char* block,
        std::array<uint32_t, 8>& hash
    ) {

        uint32_t W[64];

        // Convert 512-bit block into 16 words
        for (int i = 0; i < 16; i++) {

            int j = i * 4;

            W[i] =
                (static_cast<uint32_t>(block[j]) << 24) |
                (static_cast<uint32_t>(block[j + 1]) << 16) |
                (static_cast<uint32_t>(block[j + 2]) << 8) |
                static_cast<uint32_t>(block[j + 3]);
        }

        // Expand message schedule to 64 words
        for (int i = 16; i < 64; i++) {

            W[i] = gamma1(W[i - 2]) +
                   W[i - 7] +
                   gamma0(W[i - 15]) +
                   W[i - 16];
        }

        // Initialize working variables
        uint32_t a = hash[0];
        uint32_t b = hash[1];
        uint32_t c = hash[2];
        uint32_t d = hash[3];
        uint32_t e = hash[4];
        uint32_t f = hash[5];
        uint32_t g = hash[6];
        uint32_t h = hash[7];

        // Perform 64 compression rounds
        for (int i = 0; i < 64; i++) {

            uint32_t T1 =
                h + sigma1(e) +
                choose(e, f, g) +
                K[i] + W[i];

            uint32_t T2 =
                sigma0(a) +
                majority(a, b, c);

            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        // Update hash values
        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
        hash[5] += f;
        hash[6] += g;
        hash[7] += h;
    }

    // Convert the eight hash words into hexadecimal
    std::string toHex(const std::array<uint32_t, 8>& hash) {

        std::ostringstream output;

        output << std::hex << std::setfill('0');

        for (uint32_t value : hash) {
            output << std::setw(8) << value;
        }

        return output.str();
    }

}


// Generating SHA-256 hash for one input string
bool HashGenerator::generateHash(
    const std::string& data,
    std::string& hash
) const {

    std::array<uint32_t, 8> currentHash = INITIAL_HASH;

    const unsigned char* input =
        reinterpret_cast<const unsigned char*>(data.data());

    const size_t length = data.size();

    // Process complete 512-bit blocks
    size_t fullBlocks = length / 64;

    for (size_t i = 0; i < fullBlocks; i++) {

        processBlock(input + i * 64, currentHash);
    }

    // Prepare remaining bytes and SHA-256 padding
    std::array<unsigned char, 128> finalBlock{};

    size_t remaining = length % 64;

    if (remaining > 0) {
        std::memcpy(
            finalBlock.data(),
            input + fullBlocks * 64,
            remaining
        );
    }

    // Append the mandatory 1 bit
    finalBlock[remaining] = 0x80;

    // Append original message length in bits
    uint64_t bitLength =
        static_cast<uint64_t>(length) * 8ULL;

    size_t finalBlockSize = (remaining < 56) ? 64 : 128;

    for (int i = 0; i < 8; i++) {

        finalBlock[finalBlockSize - 1 - i] =
            static_cast<unsigned char>(
                bitLength >> (i * 8)
            );
    }

    // Process final one or two blocks
    processBlock(finalBlock.data(), currentHash);

    if (finalBlockSize == 128) {
        processBlock(finalBlock.data() + 64, currentHash);
    }

    // Return hexadecimal digest
    hash = toHex(currentHash);

    return hash.size() == 64;
}


// Generate hashes for all blocks
bool HashGenerator::generateBlockHashes(
    const std::vector<Block>& blocks,
    std::vector<std::string>& hashes
) const {

    hashes.clear();
    hashes.reserve(blocks.size());

    for (const Block& block : blocks) {

        std::string blockHash;

        if (!generateHash(block.getData(), blockHash)) {

            hashes.clear();
            return false;
        }

        hashes.push_back(std::move(blockHash));
    }

    return true;
}
