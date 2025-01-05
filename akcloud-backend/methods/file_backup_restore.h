#ifndef FILE_BACKUP_RESTORE_H
#define FILE_BACKUP_RESTORE_H

#include <iostream>
#include <fstream>
#include <filesystem>

class FileBackupRestore {
public:
    static bool copyFile(const std::string &sourcePath, const std::string &backupPath);
};

#endif // FILE_BACKUP_RESTORE_H