#include "methods/file_backup_restore.h"
#include "methods/pack_file.h"
#include <iostream>

int main() {
    std::string sourceFile = "./test";
    std::string backupFile = "./backupFiles";

    // // 备份文件
    // if (FileBackupRestore::copyFile(sourceFile, backupFile)) {
    //     std::cout << "File backed up successfully." << std::endl;
    // } else {
    //     std::cout << "File backup failed." << std::endl;
    // }

    // 打包文件
    if (PackFile::packFile(sourceFile, backupFile)) {
        std::cout << "File packed successfully." << std::endl;
    } else {
        std::cout << "File pack failed." << std::endl;
    }
    return 0;
}