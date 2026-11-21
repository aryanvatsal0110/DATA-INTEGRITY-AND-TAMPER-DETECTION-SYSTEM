
#include <iostream>
#include <vector>
#include <string>

#include "FileHandler.h"
#include "FileProfiler.h"
#include "Partitioner.h"

using namespace std;

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cerr << "Usage: TestProfiler.exe <file_path>\n";
        return 1;
    }

    string filePath = argv[1];

    // STEP 1: Read file
    FileHandler handler;
    string fileData;

    if (!handler.readFile(filePath, fileData)) {
        cerr << "Error reading file!\n";
        return 1;
    }

    cout << "\n========== FILE READ SUCCESSFULLY ==========\n";
    cout << "Total bytes read: " << fileData.size() << "\n";


    // STEP 2: Profile file
    FileProfiler profiler;
    FileProfile profile;

    if (!profiler.profileFile(filePath, profile)) {
        cerr << "Error profiling file!\n";
        return 1;
    }

    cout << "\n========== FILE PROFILE ==========\n";
    cout << "File Name: " << profile.fileName << "\n";
    cout << "Extension: " << profile.extension << "\n";
    cout << "File Type: " << profile.fileType << "\n";
    cout << "File Size: " << profile.fileSize << " bytes\n";

    if (profile.fileSize != fileData.size()) {
        cerr << "File size mismatch!\n";
        return 1;
    }


    // STEP 3: Partition file
    Partitioner partitioner;

    vector<Block> blocks;
    PartitionConfig config;

    partitioner.partitionFile(fileData, blocks, config);

    cout << "\n========== PARTITION DETAILS ==========\n";
    cout << "Selected Block Size: "
         << config.blockSize << " bytes\n";

    cout << "Total Blocks: "
         << config.totalBlocks << "\n";

    cout << "\n========== BLOCK INFORMATION ==========\n";

    for (const Block& block : blocks) {

        cout << "Block Index: "
             << block.getBlockIndex() << "\n";

        cout << "Starting Position: "
             << block.getStartPosition() << "\n";

        cout << "Block Size: "
             << block.getBlockSize() << " bytes\n";

        cout << "-----------------------------\n";
    }


    // STEP 4: Validate partitioning
    size_t totalPartitionedBytes = 0;

    for (const Block& block : blocks) {
        totalPartitionedBytes += block.getBlockSize();
    }

    cout << "\n========== VALIDATION ==========\n";

    cout << "Original File Size: "
         << fileData.size() << " bytes\n";

    cout << "Partitioned Data Size: "
         << totalPartitionedBytes << " bytes\n";

    if (totalPartitionedBytes == fileData.size() &&
        blocks.size() == config.totalBlocks) {

        cout << "\nSUCCESS: Partitioning validated!\n";
    }
    else {
        cerr << "\nERROR: Partitioning validation failed!\n";
        return 1;
    }

    return 0;
}
