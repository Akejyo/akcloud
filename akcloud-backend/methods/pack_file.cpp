#include "pack_file.h"
#include "check_file.h"
namespace fs = std::filesystem;
bool PackFile::packFile(const std::string &filePath, std::string packFilePath) {
    auto nowTime = std::chrono::system_clock::now();
    auto nowTime_t = std::chrono::system_clock::to_time_t(nowTime);
    std::tm now_tm = *std::localtime(&nowTime_t);

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d_%H:%M:%S");
    std::string timeStr = oss.str();

    // pack_time.akpk
    // 展开用户目录
    if (packFilePath[0] == '~') {
        const char *homeDir = std::getenv("HOME");
        if (homeDir) {
            packFilePath.replace(0, 1, homeDir);
        }
    }

    // 将 packFilePath 转换为绝对路径
    std::filesystem::path packFilePathObj(packFilePath);
    if (!packFilePathObj.is_absolute()) {
        packFilePathObj = std::filesystem::absolute(packFilePathObj);
    }
    std::cout << "Absolute pack file path: " << packFilePathObj << std::endl;

    // 检查 packFilePath 是否存在，如果不存在就创建它
    if (!std::filesystem::exists(packFilePathObj)) {
        std::filesystem::create_directories(packFilePathObj);
    }
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

            // Add file identifier
            char fileIdentifier = 'F';
            packFile.write(&fileIdentifier, sizeof(fileIdentifier));
            // Add filename and size
            std::string relativePath = std::filesystem::relative(entry.path(), filePathObj).string();
            std::uint64_t fileSize = std::filesystem::file_size(entry.path());
            packFile.write(relativePath.c_str(), relativePath.size() + 1); //'\0'
            packFile.write(reinterpret_cast<const char *>(&fileSize), sizeof(fileSize));

            packFile << srcFile.rdbuf();
        }
        // directory
        else if (entry.is_directory()) {
            // Add directory identifier
            char dirIdentifier = 'D';
            packFile.write(&dirIdentifier, sizeof(dirIdentifier));

            std::string dirName = entry.path().filename().string();
            std::uint64_t dirSize = static_cast<std::uint64_t>(-1);
            packFile.write(dirName.c_str(), dirName.size() + 1); //'\0'
            packFile.write(reinterpret_cast<const char *>(&dirSize), sizeof(dirSize));
        }
    }

    // Add CRC
    packFile.close();

    std::ifstream packFileIn(packFilePathObj, std::ios::binary);
    if (!packFileIn.is_open()) {
        std::cerr << "Failed to open pack file for reading." << std::endl;
        return false;
    }

    std::vector<char> buff(std::filesystem::file_size(packFilePathObj));
    packFileIn.read(buff.data(), buff.size());
    packFileIn.close();

    std::uint32_t crc = CheckFile::generateCRC(buff);

    // 重新打开文件以写入 CRC
    std::ofstream packFileOut(packFilePathObj, std::ios::binary | std::ios::app);
    if (!packFileOut.is_open()) {
        std::cerr << "Failed to open pack file for writing." << std::endl;
        return false;
    }
    packFileOut.write(reinterpret_cast<const char *>(&crc), sizeof(crc));
    packFileOut.close();

    return true;
}

void create_unpack_directory(const fs::path &unpackFilePathObj) {
    try {
        if (fs::create_directories(unpackFilePathObj)) {
            std::cout << "Directory created successfully: " << unpackFilePathObj << std::endl;
        } else {
            std::cerr << "Failed to create directory: " << unpackFilePathObj << std::endl;
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "General error: " << e.what() << std::endl;
    }
}
// Unpack file
bool PackFile::unpackFile(const std::string &packFilePath, const std::string &unpackFilePath) {
    std::filesystem::path packFilePathObj(packFilePath);
    std::filesystem::path unpackFilePathObj(unpackFilePath);

    if (!std::filesystem::exists(packFilePathObj)) {
        std::cerr << "Pack file does not exist." << std::endl;
        return false;
    }

    std::ifstream packFile(packFilePathObj, std::ios::binary);
    if (!packFile.is_open()) {
        std::cerr << "Failed to open pack file." << std::endl;
        return false;
    }

    // Check CRC
    if (!CheckFile::checkFile(packFilePathObj.string())) {
        std::cerr << "CRC check failed." << std::endl;
        return false;
    }

    // 去掉CRC长度
    std::streamoff fileSize = std::filesystem::file_size(packFilePathObj) - sizeof(std::uint32_t);
    packFile.seekg(0, std::ios::beg);
    create_unpack_directory(unpackFilePathObj);

    while (packFile.tellg() < fileSize) {
        char identifier;
        packFile.read(&identifier, sizeof(identifier));
        if (packFile.eof()) break;

        std::string name;
        std::uint64_t size;
        std::string relativePath;
        std::filesystem::path filePath;
        switch (identifier) {
        case 'F': {
            std::getline(packFile, relativePath, '\0');
            packFile.read(reinterpret_cast<char *>(&size), sizeof(size));
            filePath = unpackFilePathObj / relativePath;
            std::filesystem::create_directories(filePath.parent_path());
            std::ofstream dstFile(filePath, std::ios::binary);
            if (!dstFile.is_open()) {
                std::cerr << "Failed to open destination file: " << filePath << std::endl;
                return false;
            }

            // buffer
            const std::size_t buffSize = 4096;
            std::vector<char> buffer(buffSize);
            std::uint64_t remainSize = size;
            while (remainSize > 0) {
                std::size_t readSize = std::min(remainSize, static_cast<std::uint64_t>(buffSize));
                packFile.read(buffer.data(), readSize);
                dstFile.write(buffer.data(), readSize);
                remainSize -= readSize;
            }

            break;
        }
        case 'D': {
            std::getline(packFile, name, '\0');
            packFile.read(reinterpret_cast<char *>(&size), sizeof(size));
            filePath = unpackFilePathObj / name;
            std::filesystem::create_directories(filePath);
            break;
        }
        default:
            std::cerr << "Unknown identifier: " << identifier << std::endl;
            return false;
        }
    }
    return true;
}