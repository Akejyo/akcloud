#ifndef CHECK_FILE_H
#define CHECK_FILE_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

class CheckFile {
public:
    static std::uint32_t generateCRC(const std::vector<char> &data);
    static bool checkFile(const std::string &filePath);

private:
    static std::uint32_t crc32(const void *data, std::size_t length);
    static std::uint32_t crc32_table[256];
    static void generate_crc32_table();
};

#endif // CHECK_FILE_H