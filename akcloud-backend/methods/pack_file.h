#ifndef PACK_FILE_H
#define PACK_FILE_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <vector>
#include <algorithm>

class PackFile {
public:
    static bool packFile(const std::string &filePath, const std::string packFilePath);
    static bool unpackFile(const std::string &packFilePath, const std::string &unpackFilePath);
};

#endif // PACK_FILE_H