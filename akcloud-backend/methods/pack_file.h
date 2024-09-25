#ifndef PACK_FILE_H
#define PACK_FILE_H

#include <iostream>
#include <fstream>
#include <filesystem>

class PackFile {
public:
    static bool packFile(const std::string &sourcePath, const std::string &backupPath);
};

#endif