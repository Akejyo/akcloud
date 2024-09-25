#include "pack_file.h"

bool pack_file(const std::string &filePath, const std::string packFilePath) {
    std::filesystem::path filePathObj(filePath);
    std::ofstream packFile(packFilePath, std::ios::binary);
    if (!packFile.is_open()) {
        std::cerr << "Failed to open pack file." << std::endl;
        return false;
    }
    for (const auto &entry : std::filesystem::recursive_directory_iterator(filePathObj)) {
        // file
        if (entry.is_regular_file()) {
            std::ifstream srcFile(entry.path(), std::ios::binary);
            if (!srcFile.is_open()) {
                std::cerr << "Failed to open source file: " << entry.path() << std::endl;
                continue;
            }

            // Add filename and size
            std::string filename = entry.path().filename().string();
            std::uint64_t fileSize = std::filesystem::file_size(entry.path());
            packFile.write(filename.c_str(), filename.size() + 1); //'\0'
            packFile.write(reinterpret_cast<const char *>(&fileSize), sizeof(fileSize));

            packFile << srcFile.rdbuf();
        }
        // directory
        else if (entry.is_directory()) {
        }
    }
}