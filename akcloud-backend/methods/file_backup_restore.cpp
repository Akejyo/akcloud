#include "file_backup_restore.h"

void copyFile_(const std::filesystem::path &source, const std::filesystem::path &destination) {
    std::ifstream src(source, std::ios::binary);
    std::ofstream dst(destination, std::ios::binary);
    dst << src.rdbuf();
}

bool FileBackupRestore::copyFile(const std::string &sourcePath, const std::string &backupPath) {
    std::filesystem::path sourcePathObj(sourcePath);
    std::filesystem::path backupPathObj(backupPath);

    // File not found
    if (!std::filesystem::exists(sourcePathObj)) {
        std::cerr << "Source file does not exist." << std::endl;
        return false;
    }

    // Create backup directory if not exist
    if (!std::filesystem::exists(backupPathObj.parent_path())) {
        std::filesystem::create_directories(backupPathObj.parent_path());
    }

    // Backup Directory
    if (std::filesystem::is_directory(sourcePathObj)) {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(sourcePathObj)) {
            std::filesystem::path relativePath = std::filesystem::relative(entry.path(), sourcePathObj); // relative path from sourcePathObj
            std::filesystem::path backupPathForEntry = backupPathObj / relativePath;                     // path in backupPath
            if (std::filesystem::is_directory(entry.path())) {
                std::filesystem::create_directories(backupPathForEntry);
            } else {
                copyFile_(entry.path(), backupPathForEntry);
            }
        }
    } // Backup File
    else {
        std::filesystem::path backupFilePath = backupPathObj / sourcePathObj.filename();
        copyFile_(sourcePathObj, backupFilePath);
    }

    return true;
}
