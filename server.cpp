
#include "httplib.h"
#include "TemperDProcessor.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <chrono>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <system_error>
#include <vector>
#include <mutex>
#include <set>
#include <map>
#include <stdexcept>
#include <iterator>

namespace fs = std::filesystem;


// ==========================================
// 1. GLOBAL UTILITIES
// ==========================================

static std::atomic<unsigned long long> uploadCounter{0};


// Escape strings before placing them inside JSON
std::string jsonEscape(const std::string& value)
{
    std::ostringstream escaped;

    for (unsigned char ch : value)
    {
        switch (ch)
        {
            case '"':  escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '\b': escaped << "\\b"; break;
            case '\f': escaped << "\\f"; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;

            default:
                if (ch < 0x20)
                {
                    escaped << "\\u"
                            << std::hex
                            << std::setw(4)
                            << std::setfill('0')
                            << static_cast<int>(ch)
                            << std::dec;
                }
                else
                {
                    escaped << static_cast<char>(ch);
                }
        }
    }

    return escaped.str();
}


std::string jsonError(const std::string& message)
{
    return
        "{\"success\":false,\"message\":\"" +
        jsonEscape(message) +
        "\"}";
}


// ==========================================
// 2. DASHBOARD STATISTICS MANAGER
// ==========================================

class DashboardStats
{
private:

    std::mutex mutex;

    // All successfully registered file IDs.
    std::set<std::string> registeredFiles;

    // Latest verification status of each registered file.
    // true = Authentic
    // false = Tampered
    std::map<std::string, bool> latestVerification;

    // Counts every successful verification operation.
    unsigned long long totalVerifications = 0;

    const fs::path statsPath =
        "storage/dashboard_stats.dat";


    // Save statistics to persistent storage.
    void saveUnlocked()
    {
        fs::path temporaryPath =
            "storage/dashboard_stats.tmp";

        std::ofstream output(
            temporaryPath,
            std::ios::trunc
        );

        if (!output.is_open())
        {
            throw std::runtime_error(
                "Unable to save dashboard statistics."
            );
        }

        output << "TEMPERD_STATS_V1\n";

        output << "C "
               << totalVerifications
               << "\n";

        for (const auto& fileId : registeredFiles)
        {
            output << "R "
                   << std::quoted(fileId)
                   << "\n";
        }

        for (const auto& entry : latestVerification)
        {
            output << "V "
                   << std::quoted(entry.first)
                   << " "
                   << (entry.second ? 1 : 0)
                   << "\n";
        }

        output.close();

        if (!output)
        {
            throw std::runtime_error(
                "Unable to write dashboard statistics."
            );
        }

        std::error_code error;

        fs::remove(statsPath, error);

        error.clear();

        fs::rename(
            temporaryPath,
            statsPath,
            error
        );

        if (error)
        {
            fs::remove(temporaryPath);

            throw std::runtime_error(
                "Unable to update dashboard statistics."
            );
        }
    }


public:

    // Load statistics when the server starts.
    void load()
    {
        std::lock_guard<std::mutex> lock(mutex);

        registeredFiles.clear();
        latestVerification.clear();

        totalVerifications = 0;

        std::ifstream input(statsPath);

        if (!input.is_open())
        {
            return;
        }

        std::string header;

        std::getline(input, header);

        if (header != "TEMPERD_STATS_V1")
        {
            std::cerr
                << "Warning: Invalid dashboard statistics file."
                << "\n";

            return;
        }

        std::string line;

        while (std::getline(input, line))
        {
            std::istringstream record(line);

            char type;

            if (!(record >> type))
            {
                continue;
            }

            if (type == 'C')
            {
                record >> totalVerifications;
            }

            else if (type == 'R')
            {
                std::string fileId;

                if (record >> std::quoted(fileId))
                {
                    registeredFiles.insert(fileId);
                }
            }

            else if (type == 'V')
            {
                std::string fileId;

                int authentic = 0;

                if (record >> std::quoted(fileId) >> authentic)
                {
                    latestVerification[fileId] =
                        (authentic == 1);
                }
            }
        }

        std::cout
            << "\nDashboard statistics loaded successfully."
            << "\n";
    }


    // Record a successful registration.
    void recordRegistration(const std::string& fileId)
    {
        std::lock_guard<std::mutex> lock(mutex);

        registeredFiles.insert(fileId);

        try
        {
            saveUnlocked();
        }
        catch (const std::exception& error)
        {
            std::cerr
                << "Statistics save error: "
                << error.what()
                << "\n";
        }
    }


    // Record a successful verification.
    void recordVerification(
        const std::string& fileId,
        bool authentic
    )
    {
        std::lock_guard<std::mutex> lock(mutex);

        latestVerification[fileId] = authentic;

        totalVerifications++;

        try
        {
            saveUnlocked();
        }
        catch (const std::exception& error)
        {
            std::cerr
                << "Statistics save error: "
                << error.what()
                << "\n";
        }
    }


    // Generate dashboard statistics JSON.
    std::string toJson()
    {
        std::lock_guard<std::mutex> lock(mutex);

        size_t authenticCount = 0;

        size_t tamperedCount = 0;

        for (const auto& entry : latestVerification)
        {
            if (entry.second)
            {
                authenticCount++;
            }
            else
            {
                tamperedCount++;
            }
        }

        std::ostringstream json;

        json << "{";

        json << "\"success\":true,";

        json << "\"registeredFiles\":"
             << registeredFiles.size()
             << ",";

        json << "\"authenticFiles\":"
             << authenticCount
             << ",";

        json << "\"tamperedFiles\":"
             << tamperedCount
             << ",";

        json << "\"totalVerifications\":"
             << totalVerifications;

        json << "}";

        return json.str();
    }
};


// ==========================================
// 3. CORS HEADERS
// ==========================================

void addCorsHeaders(httplib::Response& res)
{
    res.set_header(
        "Access-Control-Allow-Origin",
        "*"
    );

    res.set_header(
        "Access-Control-Allow-Methods",
        "GET, POST, OPTIONS"
    );

    res.set_header(
        "Access-Control-Allow-Headers",
        "Content-Type"
    );
}


// ==========================================
// 4. TEMPORARY UPLOAD PATH
// ==========================================

fs::path createTemporaryUploadPath(
    const std::string& filename
)
{
    auto timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

    unsigned long long counter =
        uploadCounter.fetch_add(1);

    std::string storedName =
        "temp_" +
        std::to_string(timestamp) +
        "_" +
        std::to_string(counter) +
        "_" +
        filename;

    return fs::path("storage/uploads") / storedName;
}


// ==========================================
// 5. SAVE UPLOADED MULTIPART FILE
// ==========================================

bool saveUploadedFile(
    const httplib::FormData& uploadedFile,
    fs::path& outputPath
)
{
    std::string filename =
        httplib::sanitize_filename(
            uploadedFile.filename
        );

    if (filename.empty())
    {
        return false;
    }

    std::error_code error;

    fs::create_directories(
        "storage/uploads",
        error
    );

    if (error)
    {
        return false;
    }

    outputPath =
        createTemporaryUploadPath(filename);

    std::ofstream output(
        outputPath,
        std::ios::binary | std::ios::trunc
    );

    if (!output.is_open())
    {
        return false;
    }

    output.write(
        uploadedFile.content.data(),
        static_cast<std::streamsize>(
            uploadedFile.content.size()
        )
    );

    output.close();

    if (!output)
    {
        fs::remove(outputPath, error);

        return false;
    }

    return true;
}


// ==========================================
// 6. SERIALIZE BLOCK CHANGES
// ==========================================

std::string serializeBlockChanges(
    const LocalizationResult& localization
)
{
    std::ostringstream json;

    json << "[";

    for (size_t i = 0;
         i < localization.changes.size();
         ++i)
    {
        const BlockChange& change =
            localization.changes[i];

        std::string type;

        switch (change.changeType)
        {
            case ChangeType::MODIFIED:
                type = "MODIFIED";
                break;

            case ChangeType::ADDED:
                type = "ADDED";
                break;

            case ChangeType::REMOVED:
                type = "REMOVED";
                break;
        }

        if (i > 0)
        {
            json << ",";
        }

        json << "{"
             << "\"blockIndex\":" << change.blockIndex << ","
             << "\"changeType\":\"" << type << "\","
             << "\"startByte\":" << change.startByte << ","
             << "\"originalSize\":" << change.originalSize << ","
             << "\"currentSize\":" << change.currentSize
             << "}";
    }

    json << "]";

    return json.str();
}


// ==========================================
// 7. SERIALIZE TEXT CHANGES
// ==========================================

std::string serializeTextChanges(
    const FileComparisonResult& comparison
)
{
    std::ostringstream json;

    json << "[";

    for (size_t i = 0;
         i < comparison.changes.size();
         ++i)
    {
        const LineChange& change =
            comparison.changes[i];

        std::string type;

        switch (change.changeType)
        {
            case LineChangeType::MODIFIED:
                type = "MODIFIED";
                break;

            case LineChangeType::ADDED:
                type = "ADDED";
                break;

            case LineChangeType::REMOVED:
                type = "REMOVED";
                break;
        }

        if (i > 0)
        {
            json << ",";
        }

        json << "{";

        json << "\"changeType\":\""
             << type
             << "\",";

        json << "\"originalLine\":"
             << change.originalLineNumber
             << ",";

        json << "\"currentLine\":"
             << change.currentLineNumber
             << ",";

        json << "\"originalText\":\""
             << jsonEscape(change.originalText)
             << "\",";

        json << "\"currentText\":\""
             << jsonEscape(change.currentText)
             << "\",";

        json << "\"removedWords\":[";

        for (size_t j = 0;
             j < change.removedWords.size();
             ++j)
        {
            if (j > 0)
                json << ",";

            json << "\""
                 << jsonEscape(change.removedWords[j])
                 << "\"";
        }

        json << "],\"addedWords\":[";

        for (size_t j = 0;
             j < change.addedWords.size();
             ++j)
        {
            if (j > 0)
                json << ",";

            json << "\""
                 << jsonEscape(change.addedWords[j])
                 << "\"";
        }

        json << "]}";
    }

    json << "]";

    return json.str();
}


// ==========================================
// 8. MAIN SERVER
// ==========================================

int main()
{
    // Create storage directories.

    fs::create_directories("storage/uploads");
    fs::create_directories("storage/original_files");
    fs::create_directories("storage/baselines");
    fs::create_directories("storage/reports");


    // Initialize processing engine.

    TemperDProcessor processor;


    // Initialize and load dashboard statistics.

    DashboardStats dashboardStats;

    dashboardStats.load();


    httplib::Server server;


    // ==========================================
    // 9. HEALTH CHECK
    // GET /api/health
    // ==========================================

    server.Get(
        "/api/health",

        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            addCorsHeaders(res);

            res.set_content(
                R"({"success":true,"message":"TemperD C++ backend is running"})",
                "application/json"
            );
        }
    );


    // ==========================================
    // 10. DASHBOARD STATISTICS
    // GET /api/stats
    // ==========================================

    server.Get(
        "/api/stats",

        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            addCorsHeaders(res);

            res.set_content(
                dashboardStats.toJson(),
                "application/json"
            );
        }
    );


    // CORS preflight for statistics.

    server.Options(
        "/api/stats",

        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            addCorsHeaders(res);

            res.status = 204;
        }
    );


    // ==========================================
    // 11. CORS PREFLIGHT
    // ==========================================

    server.Options(
        "/api/register",

        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            addCorsHeaders(res);

            res.status = 204;
        }
    );


    server.Options(
        "/api/verify",

        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            addCorsHeaders(res);

            res.status = 204;
        }
    );


    // ==========================================
    // 12. REGISTER FILE
    // POST /api/register
    // ==========================================

    server.Post(
        "/api/register",

        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            addCorsHeaders(res);

            // Validate multipart request.

            if (!req.is_multipart_form_data() ||
                !req.form.has_file("file"))
            {
                res.status = 400;

                res.set_content(
                    jsonError("No file received."),
                    "application/json"
                );

                return;
            }


            // Retrieve uploaded file.

            const auto& uploadedFile =
                req.form.get_file("file");


            // Save temporary upload.

            fs::path temporaryPath;

            if (!saveUploadedFile(
                    uploadedFile,
                    temporaryPath
                ))
            {
                res.status = 500;

                res.set_content(
                    jsonError(
                        "Unable to save uploaded file."
                    ),
                    "application/json"
                );

                return;
            }


            // Register using TemperD processing modules.

            RegistrationResult registration;

            bool success = processor.registerFile(
                temporaryPath.string(),
                "storage",
                registration
            );


            // Remove temporary upload.

            std::error_code error;

            fs::remove(temporaryPath, error);


            // Handle registration failure.

            if (!success)
            {
                res.status = 500;

                res.set_content(
                    jsonError(registration.message),
                    "application/json"
                );

                std::cout
                    << "\nRegistration failed: "
                    << registration.message
                    << "\n";

                return;
            }


            // Update dashboard statistics.

            dashboardStats.recordRegistration(
                registration.fileId
            );


            // Construct registration response.

            std::ostringstream response;

            response << "{";

            response << "\"success\":true,";

            response << "\"message\":\""
                     << jsonEscape(registration.message)
                     << "\",";

            response << "\"fileId\":\""
                     << jsonEscape(registration.fileId)
                     << "\",";

            response << "\"fileName\":\""
                     << jsonEscape(
                            httplib::sanitize_filename(
                                uploadedFile.filename
                            )
                        )
                     << "\",";

            response << "\"merkleRoot\":\""
                     << registration.merkleRoot
                     << "\",";

            response << "\"fileSize\":"
                     << registration.fileSize
                     << ",";

            response << "\"blockSize\":"
                     << registration.blockSize
                     << ",";

            response << "\"totalBlocks\":"
                     << registration.totalBlocks;

            response << "}";


            res.set_content(
                response.str(),
                "application/json"
            );


            // Console output.

            std::cout
                << "\n========== FILE REGISTERED ==========\n";

            std::cout
                << "File ID: "
                << registration.fileId
                << "\n";

            std::cout
                << "Merkle Root: "
                << registration.merkleRoot
                << "\n";

            std::cout
                << "Blocks: "
                << registration.totalBlocks
                << "\n";
        }
    );


    // ==========================================
    // 13. VERIFY FILE
    // POST /api/verify
    // ==========================================

    server.Post(
        "/api/verify",

        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            addCorsHeaders(res);


            // Validate multipart request.

            if (!req.is_multipart_form_data() ||
                !req.form.has_file("file") ||
                !req.form.has_field("fileId"))
            {
                res.status = 400;

                res.set_content(
                    jsonError(
                        "A file and registered fileId are required."
                    ),
                    "application/json"
                );

                return;
            }


            // Retrieve registered file ID.

            std::string fileId =
                req.form.get_field("fileId");

            if (fileId.empty())
            {
                res.status = 400;

                res.set_content(
                    jsonError("Invalid file ID."),
                    "application/json"
                );

                return;
            }


            // Retrieve uploaded file.

            const auto& uploadedFile =
                req.form.get_file("file");


            // Save temporary verification upload.

            fs::path temporaryPath;

            if (!saveUploadedFile(
                    uploadedFile,
                    temporaryPath
                ))
            {
                res.status = 500;

                res.set_content(
                    jsonError(
                        "Unable to save verification upload."
                    ),
                    "application/json"
                );

                return;
            }


            // Run verification and localization.

            TemperDVerificationResult verification;

            bool success = processor.verifyRegisteredFile(
                temporaryPath.string(),
                fileId,
                "storage",
                verification
            );


            // Remove temporary upload.

            std::error_code error;

            fs::remove(temporaryPath, error);


            // Handle verification failure.

            if (!success)
            {
                res.status = 400;

                res.set_content(
                    jsonError(verification.message),
                    "application/json"
                );

                return;
            }


            // Update dashboard statistics.

            dashboardStats.recordVerification(
                fileId,
                verification.verification.integrityMatch
            );


            // Construct verification response.

            std::ostringstream response;

            response << "{";

            response << "\"success\":true,";

            response << "\"message\":\""
                     << jsonEscape(verification.message)
                     << "\",";

            response << "\"fileId\":\""
                     << jsonEscape(fileId)
                     << "\",";

            response << "\"integrityMatch\":"
                     << (
                         verification.verification.integrityMatch
                         ? "true"
                         : "false"
                     )
                     << ",";

            response << "\"originalRoot\":\""
                     << verification.verification.originalRoot
                     << "\",";

            response << "\"currentRoot\":\""
                     << verification.verification.currentRoot
                     << "\",";

            response << "\"originalBlockCount\":"
                     << verification.verification.originalBlockCount
                     << ",";

            response << "\"currentBlockCount\":"
                     << verification.verification.currentBlockCount
                     << ",";

            response << "\"changedBlocks\":"
                     << serializeBlockChanges(
                            verification.localization
                        )
                     << ",";

            response << "\"textChanges\":"
                     << serializeTextChanges(
                            verification.comparison
                        )
                     << ",";

            response << "\"reportId\":\""
                     << jsonEscape(
                            verification.report.reportId
                        )
                     << "\",";

            response << "\"reportUrl\":\"/api/report/"
                     << jsonEscape(
                            verification.report.reportId
                        )
                     << "\"";

            response << "}";


            res.set_content(
                response.str(),
                "application/json"
            );


            // Console output.

            std::cout
                << "\n========== FILE VERIFIED ==========\n";

            std::cout
                << "File ID: "
                << fileId
                << "\n";

            std::cout
                << "Integrity: "
                << (
                    verification.verification.integrityMatch
                    ? "VERIFIED"
                    : "TAMPERED"
                )
                << "\n";

            std::cout
                << "Changed Blocks: "
                << verification.localization.changes.size()
                << "\n";

            std::cout
                << "Report ID: "
                << verification.report.reportId
                << "\n";
        }
    );


    // ==========================================
    // 14. RETRIEVE GENERATED REPORT
    // GET /api/report/{reportId}
    // ==========================================

    server.Get(
        R"(/api/report/([A-Za-z0-9_-]+))",

        [&](const httplib::Request& req,
            httplib::Response& res)
        {
            addCorsHeaders(res);

            std::string reportId =
                req.matches[1];

            fs::path reportPath =
                fs::path("storage/reports") /
                (reportId + ".json");


            std::ifstream input(
                reportPath,
                std::ios::binary
            );

            if (!input.is_open())
            {
                res.status = 404;

                res.set_content(
                    jsonError("Report not found."),
                    "application/json"
                );

                return;
            }


            std::string reportData(
                (std::istreambuf_iterator<char>(input)),
                std::istreambuf_iterator<char>()
            );


            res.set_content(
                reportData,
                "application/json"
            );
        }
    );


    // ==========================================
    // 15. START SERVER
    // ==========================================

    std::cout
        << "\n================================\n";

    std::cout
        << "       TEMPERD SERVER\n";

    std::cout
        << "================================\n";


    std::cout
        << "\nAvailable endpoints:\n";

    std::cout
        << "GET  /api/health\n";

    std::cout
        << "GET  /api/stats\n";

    std::cout
        << "POST /api/register\n";

    std::cout
        << "POST /api/verify\n";

    std::cout
        << "GET  /api/report/{reportId}\n";


    std::cout
        << "\nServer running at:\n";

    std::cout
        << "http://localhost:8080\n\n";


    if (!server.listen("127.0.0.1", 8080))
    {
        std::cerr
            << "ERROR: Unable to start TemperD server.\n";

        return 1;
    }

    return 0;
}
