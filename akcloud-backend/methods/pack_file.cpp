#include "pack_file.h"

bool PackFile::packFile(const std::string &filePath, const std::string packFilePath) {
    auto nowTime = std::chrono::system_clock::now();
    auto nowTime_t = std::chrono::system_clock::to_time_t(nowTime);
    std::tm now_tm = *std::localtime(&nowTime_t);

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d_%H:%M:%S");
    std::string timeStr = oss.str();

    // pack_time.akpk
    std::filesystem::path packFilePathObj(packFilePath);
    packFilePathObj /= "pack_" + timeStr + ".akpk";

    std::filesystem::create_directories(packFilePathObj.parent_path());
    std::ofstream packFile(packFilePathObj, std::ios::binary);
    if (!packFile.is_open()) {
        std::cerr << "Failed to open pack file." << std::endl;
        return false;
    }

    std::filesystem::path filePathObj(filePath);
    for (const auto &entry : std::filesystem::recursive_directory_iterator(filePathObj)) {
        // file
        if (entry.is_regular_file()) {
            std::ifstream srcFile(entry.path(), std::ios::binary);
            if (!srcFile.is_open()) {
                std::cerr << "Failed to open source file: " << entry.path() << std::endl;
                continue;
            }

            // Add filename and size
            std::string relativePath = std::filesystem::relative(entry.path(), filePathObj).string();
            std::uint64_t fileSize = std::filesystem::file_size(entry.path());
            packFile.write(relativePath.c_str(), relativePath.size() + 1); //'\0'
            packFile.write(reinterpret_cast<const char *>(&fileSize), sizeof(fileSize));

            packFile << srcFile.rdbuf();
        }
        // directory
        else if (entry.is_directory()) {
            std::string dirName = entry.path().filename().string();
            std::uint64_t dirSize = static_cast<std::uint64_t>(-1);
            packFile.write(dirName.c_str(), dirName.size() + 1); //'\0'
            packFile.write(reinterpret_cast<const char *>(&dirSize), sizeof(dirSize));
        }
    }
    return true;
}