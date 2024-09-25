#ifndef PACK_FILE_H
#define PACK_FILE_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <chrono>

class PackFile {
public:
    static bool packFile(const std::string &filePath, const std::string packFilePath);
};

#endif // PACK_FILE_H