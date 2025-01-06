#include "methods/file_backup_restore.h"
#include "methods/pack_file.h"
#include "methods/check_file.h"
#include <iostream>

int main() {
    std::string sourceFile = "./test";
    std::string backupFile = "./backupFiles";
    std::string unpackFile = "./unpackFiles";

    // // 备份文件
    // if (FileBackupRestore::copyFile(sourceFile, backupFile)) {
    //     std::cout << "File backed up successfully." << std::endl;
    // } else {
    //     std::cout << "File backup failed." << std::endl;
    // }

    // // 打包文件
    // if (PackFile::packFile(sourceFile, backupFile)) {
    //     std::cout << "File packed successfully." << std::endl;
    // } else {
    //     std::cout << "File pack failed." << std::endl;
    // }

    // 解包文件
    if (PackFile::unpackFile(backupFile + "/backup_test.akpk", "unpackFiles")) {
        std::cout << "File unpacked successfully." << std::endl;
    } else {
        std::cout << "File unpack failed." << std::endl;
    }

    return 0;
}