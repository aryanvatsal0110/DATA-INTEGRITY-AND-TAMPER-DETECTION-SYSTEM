# Data Integrity and Tamper Detection System

An integrated data authentication and tamper detection system designed to verify file integrity, identify unauthorized modifications, and generate detailed tamper reports using cryptographic hashing and Merkle Tree-based verification.

## 1. Project Overview

The Data Integrity and Tamper Detection System aims to provide a structured approach to data integrity verification.

The system processes input files, generates cryptographic hashes, constructs Merkle Trees, maintains trusted baselines, and verifies data against previously stored references.

It also supports tamper localization and report generation, helping identify changes at the file or record level instead of relying only on a single overall integrity result.

## 2. Key Features

- **Cryptographic Hashing:** Generate hash values for data authentication.
- **Merkle Tree Construction:** Organize hashes into a tree structure and calculate root hashes.
- **Hierarchical Merkle Trees:** Support hierarchical organization of authenticated data.
- **Input Profiling:** Analyze input files before processing.
- **Data Partitioning:** Divide input data into manageable blocks.
- **Baseline Management:** Store trusted reference data for subsequent verification.
- **Integrity Verification:** Compare current data against trusted baselines.
- **Tamper Detection:** Identify unauthorized modifications.
- **Tamper Localization:** Compare file contents to identify affected portions.
- **Automated Reporting:** Generate reports containing verification and tamper information.
- **Web Interface:** Provide a browser-based interface for interacting with the system.

## 3. System Architecture

The project follows a modular architecture, separating major responsibilities into independent components.

### Module 1: Hashing and Tree Construction

Directory: `Hashing_Tree_Construction/`

Responsible for:
- Hash generation.
- Merkle Node management.
- Merkle Tree construction.
- Hierarchical Merkle Tree construction.
- Hashing and tree-related testing.

### Module 2: Input Profiling and Partitioning

Directory: `Input_Profiling_Partitioning/`

Responsible for:
- Input file handling.
- File profiling.
- Data block representation.
- Data partitioning.
- Input processing tests.

### Module 3: Localization and Reporting

Directory: `Localization_Report/`

Responsible for:
- File comparison.
- Tamper detection.
- Identification of modified content.
- Report generation.
- Component-level testing.

### Module 4: Root Verification

Directory: `Root_Verification/`

Responsible for:
- Baseline storage.
- Root management.
- Integrity verification.
- Verification testing.

### Module 5: TemperD Processing

Directory: `TemperD/`

Responsible for:
- Processing operations through the TemperD processor.
- Dedicated processor testing.

## 4. Project Structure

```text
DATA-INTEGRITY-AND-TAMPER-DETECTION-SYSTEM/
│
├── Hashing_Tree_Construction/
│   ├── HashGenerator.cpp
│   ├── HashGenerator.h
│   ├── MerkleTree.cpp
│   ├── MerkleTree.h
│   ├── MerkleNode.h
│   ├── HierarchicalMerkleTree.cpp
│   ├── HierarchicalMerkleTree.h
│   └── test files
│
├── Input_Profiling_Partitioning/
│   ├── Block.h
│   ├── FileHandler.cpp
│   ├── FileHandler.h
│   ├── FileProfiler.cpp
│   ├── FileProfiler.h
│   ├── Partitioner.cpp
│   ├── Partitioner.h
│   └── test.cpp
│
├── Localization_Report/
│   ├── FileComparator.cpp
│   ├── FileComparator.h
│   ├── TamperDetector.cpp
│   ├── TamperDetector.h
│   ├── ReportGenerator.cpp
│   ├── ReportGenerator.h
│   └── test files
│
├── Root_Verification/
│   ├── BaselineStorage.cpp
│   ├── BaselineStorage.h
│   ├── IntegrityVerifier.cpp
│   ├── IntegrityVerifier.h
│   ├── RootManager.cpp
│   ├── RootManager.h
│   └── test_verification.cpp
│
├── TemperD/
│   ├── TemperDProcessor.cpp
│   ├── TemperDProcessor.h
│   └── test_temperd_processor.cpp
│
├── storage/
│   ├── baselines/
│   ├── original_files/
│   ├── reports/
│   └── uploads/
│
├── .vscode/
│
├── httplib.h
├── server.cpp
├── index.html
├── style.css
└── script.js
```

## 5. Technology Stack

| Component | Technology |
|---|---|
| Core Development | C++ |
| Cryptographic Hashing | SHA-256 |
| Frontend | HTML, CSS, JavaScript |
| Backend | C++ |
| HTTP Support | cpp-httplib header |
| Data Storage | File System, JSON and binary files |
| Development Environment | Visual Studio Code |
| Version Control | Git and GitHub |

## 6. Storage Organization

The `storage/` directory maintains the files required by the application.

- `baselines/` – Stored baseline data used during verification.
- `original_files/` – Original input files.
- `reports/` – Generated verification and tamper reports.
- `uploads/` – Files uploaded for testing and verification.

## 7. Testing

The project contains dedicated test programs alongside the individual modules.

Testing areas include:

- Hash generation.
- Merkle Tree construction.
- Hierarchical Merkle Tree operations.
- Input handling and partitioning.
- File comparison.
- Tamper detection.
- Report generation.
- Baseline storage and integrity verification.
- TemperD processing.

## 8. Getting Started

### Prerequisites

- Windows, Linux or another supported development environment.
- C++ compiler.
- Visual Studio Code (recommended).
- A modern web browser.

### Clone the Repository

```bash
git clone https://github.com/aryanvatsal0110/DATA-INTEGRITY-AND-TAMPER-DETECTION-SYSTEM.git

cd DATA-INTEGRITY-AND-TAMPER-DETECTION-SYSTEM
```

### Running the Application

1. Open the project directory in Visual Studio Code.
2. Ensure that the required C++ compiler and dependencies are configured.
3. Configure and compile the C++ backend using the project's build configuration.
4. Start the backend server.
5. Open the frontend through the configured local server address in your browser.

*Note: The exact compilation and server execution commands depend on the project's configured build and runtime settings.*

## 9. Repository Collaboration

This project is developed collaboratively using Git and GitHub.

The repository maintains modular development, independent testing, integration and version-controlled contributions.

## 10. Project Objective

The primary objective is to establish a reliable data authentication workflow that combines cryptographic hashing, Merkle Tree structures, baseline management and tamper localization.

The system aims to make integrity verification more structured, identify affected data efficiently, and provide meaningful reports when unauthorized modifications are detected.

---

**Data Integrity and Tamper Detection System**

*Securing Data Through Cryptographic Authentication and Verification.*
