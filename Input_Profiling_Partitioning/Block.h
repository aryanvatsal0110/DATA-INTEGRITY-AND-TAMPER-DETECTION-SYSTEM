
#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <cstddef>

class Block {
private:
    int blockIndex;
    size_t startPosition;
    size_t blockSize;
    std::string data;

public:
    Block(int index, size_t start, const std::string& content)
        : blockIndex(index),
          startPosition(start),
          blockSize(content.size()),
          data(content) {
    }

    int getBlockIndex() const {
        return blockIndex;
    }

    size_t getStartPosition() const {
        return startPosition;
    }

    size_t getBlockSize() const {
        return blockSize;
    }

    const std::string& getData() const {
        return data;
    }
};

#endif
