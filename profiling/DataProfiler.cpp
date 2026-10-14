#include "DataProfiler.h"

#include <iostream>
#include <fstream>

using namespace std;

DataProfile profileDataset(
    const string& filename,
    const vector<string>& records)
{
    DataProfile profile;

    profile.fileName = filename;

    // -----------------------------
    // Determine file type
    // -----------------------------

    size_t position = filename.find_last_of('.');

    if (position != string::npos)
    {
        profile.fileType =
            filename.substr(position + 1);
    }
    else
    {
        profile.fileType = "Unknown";
    }

    // -----------------------------
    // Determine file size
    // -----------------------------

    ifstream file(filename, ios::binary | ios::ate);

    if (file.is_open())
    {
        profile.fileSize = file.tellg();
        file.close();
    }
    else
    {
        profile.fileSize = 0;
    }

    // -----------------------------
    // Number of records
    // -----------------------------

    profile.recordCount =
        records.size();

    // -----------------------------
    // Average record size
    // -----------------------------

    if (profile.recordCount > 0)
    {
        long long totalSize = 0;

        for (const string& record : records)
        {
            totalSize += record.size();
        }

        profile.averageRecordSize =
            static_cast<double>(totalSize)
            / profile.recordCount;
    }
    else
    {
        profile.averageRecordSize = 0;
    }

    return profile;
}


void displayProfile(const DataProfile& profile)
{
    cout << "\n========== DATA PROFILE ==========\n";

    cout << "File Name           : "
         << profile.fileName << endl;

    cout << "File Type           : "
         << profile.fileType << endl;

    cout << "File Size           : "
         << profile.fileSize
         << " bytes" << endl;

    cout << "Number of Records   : "
         << profile.recordCount << endl;

    cout << "Average Record Size : "
         << profile.averageRecordSize
         << " bytes" << endl;

    cout << "==================================\n";
}
