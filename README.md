# Data Authentication and Tamper Detection System Using an Adaptive Merkle Tree

## Project Overview

The Data Authentication and Tamper Detection System Using an Adaptive Merkle Tree is a C++-based project designed to verify data integrity, detect unauthorized modifications, and identify affected data blocks or records. The system combines SHA-256 cryptographic hashing, adaptive data partitioning, and Merkle Tree-based authentication to establish a trusted reference for data verification.

## Problem Statement

Traditional data integrity verification methods may require repeated processing of large datasets and may not efficiently identify affected records after unauthorized modifications. This project aims to combine cryptographic hashing, adaptive data partitioning, and Merkle Tree authentication to detect tampering, verify data integrity, and localize affected records.

## Objectives

- Implement SHA-256 hashing for data integrity verification.
- Develop adaptive data partitioning based on dataset characteristics.
- Construct Merkle Trees for structured hash-based authentication.
- Generate and verify Merkle proofs for data blocks.
- Detect unauthorized modifications and identify affected records.
- Generate tamper reports and evaluate performance across different dataset sizes.

## Key Features

- **File Handling:** Read and process input datasets.
- **Data Profiling:** Analyze file size, record count, and average record size.
- **Adaptive Partitioning:** Divide records into manageable blocks.
- **SHA-256 Hashing:** Generate cryptographic hash values.
- **Merkle Tree Authentication:** Organize hashes into a tree structure and calculate a root hash.
- **Merkle Proof Verification:** Verify individual blocks against a trusted Merkle root.
- **Tamper Localization:** Identify potentially modified, added, or deleted records.
- **Tamper Reporting:** Present verification results and detected changes.

## Technology Stack

- **Programming Language:** C++
- **Cryptographic Algorithm:** SHA-256
- **Data Structures:** Vectors, structures, and Merkle Trees
- **File Handling:** C++ file streams
- **Development Environment:** Visual Studio Code
- **Version Control:** Git and GitHub
- **Compiler:** g++ (if used)

## System Workflow

1. Read the input dataset.
2. Analyze the dataset using data profiling.
3. Divide the dataset into blocks using the partitioning policy.
4. Generate SHA-256 hashes for the data blocks.
5. Construct the Merkle Tree and calculate the root hash.
6. Generate Merkle proofs for selected blocks.
7. Verify the dataset against a trusted reference.
8. Detect hash mismatches and identify affected records.
9. Generate a tamper report.

## Project Structure

The project follows a modular architecture to support independent development, testing, and integration.

```text
DataAuthentication/
│
├── main.cpp
│
├── input/
│   ├── InputHandler.h
│   └── InputHandler.cpp
│
├── profiling/
│   ├── DataProfiler.h
│   └── DataProfiler.cpp
│
├── partitioning/
│   ├── Partitioner.h
│   └── Partitioner.cpp
│
├── hashing/
│   ├── SHA256.h
│   └── SHA256.cpp
│
├── tree/
│   ├── MerkleNode.h
│   ├── BinaryMerkleTree.h
│   ├── BinaryMerkleTree.cpp
│   ├── HierarchicalMerkleTree.h
│   └── HierarchicalMerkleTree.cpp
│
├── proof/
│   ├── MerkleProof.h
│   └── MerkleProof.cpp
│
├── verification/
│   ├── Verification.h
│   └── Verification.cpp
│
├── localization/
│   ├── TamperLocalization.h
│   └── TamperLocalization.cpp
│
├── report/
│   ├── TamperReport.h
│   └── TamperReport.cpp
│
├── data/
│   └── dataset.csv
│
└── output/
```

## Installation and Execution

### Prerequisites

- A compatible C++ compiler.
- Git installed on the system.
- Visual Studio Code or another C++ development environment.

### Clone the Repository

```bash
git clone https://github.com/aryanvatsal0110/DATA-INTEGRITY-AND-TAMPER-DETECTION-SYSTEM
cd DataAuthentication
```

### Compile and Run

Compile the project using the appropriate command for the source files and libraries implemented in the repository.

For a g++-based project, the command may follow this pattern:

```bash
g++ -std=c++11 main.cpp input/InputHandler.cpp profiling/DataProfiler.cpp partitioning/Partitioner.cpp hashing/SHA256.cpp localization/TamperLocalization.cpp report/TamperReport.cpp -o data_auth
```

Run on Windows:

```powershell
.\data_auth.exe
```

Run on Linux:

```bash
./data_auth
```

## Testing and Evaluation

The following test cases are intended to evaluate the system:

- **Untampered Data:** Verify that unchanged data matches the trusted reference.
- **Modified Data:** Change a record and check whether a hash mismatch is detected.
- **Added Data:** Add a record and check whether the modification is detected.
- **Deleted Data:** Remove a record and check the verification result.
- **Dataset Size Evaluation:** Test datasets of different sizes.
- **Performance Evaluation:** Measure tree construction time, verification time, and tamper localization time.

Actual results should be documented after executing and validating these tests.

## Development Status

The project is being developed using a modular approach.

- Input handling and data profiling.
- Adaptive data partitioning.
- SHA-256 hashing.
- Merkle Tree construction and proof verification.
- Tamper localization and report generation.
- End-to-end integration and performance evaluation.

The implementation status of each module should be updated according to its actual development and testing progress.

## Expected Outcomes

- Cryptographic verification of data integrity.
- Detection of unauthorized data modifications.
- Identification of affected data blocks or records.
- Merkle proof-based verification against a trusted root.
- Generation of tamper reports.
- Performance evaluation across datasets of different sizes.

## Future Scope

- Improve adaptive partitioning using additional dataset characteristics.
- Optimize hierarchical Merkle Tree construction for large datasets.
- Support additional structured file formats.
- Improve tamper localization through subtree verification.
- Develop a web-based interface for file upload and integrity verification.
- Add performance visualizations and detailed verification reports.
- Explore incremental updates for frequently changing datasets.

## Team Contributions

| Module / Task | Team Member | Contribution |
|---|---|---|
| Data Profiling and Adaptive Partitioning | Agrim | Dataset profiling and adaptive data block partitioning |
| Merkle Tree and Root Verification | Aryan Vatsal | Tree construction and root hash verification |
| SHA-256 Hashing and Proof Generation | Gagandeep Kaur | Cryptographic hashing and Merkle proof generation |
| Tamper Localization and Report Generation | Aditi | Identification of modified records and generation of tamper reports |

### Collaborative Development

All team members contribute to modular development, testing, integration, documentation, and version control through GitHub.

## Conclusion

This project proposes a modular data authentication system combining SHA-256 cryptographic hashing, adaptive data partitioning, and Merkle Tree-based verification. Its goal is to detect unauthorized modifications, verify data integrity, and identify affected records while evaluating performance across datasets of different sizes.
