#include <iostream>
#include "methods/file_backup_restore.h"
#include "methods/pack_file.h"
#include "methods/check_file.h"
#include "methods/encrypt.h"
#include "methods/decomposer.h"
#include "methods/composer.h"
#include <filesystem>
#include <vector>
#include <string>
#include <ctime>
#include <nlohmann/json.hpp>
#include <fstream>
#include <httplib.h>
namespace fs = std::filesystem;

struct FileInfo {
    std::string name;
    bool isDirectory;
    std::uintmax_t size;
    std::string lastModified;
};

void copy_files_to_pack(const std::vector<std::string> &files, const fs::path &backup_path, const fs::path &pack_path) {
    for (const auto &file : files) {
        fs::path source = backup_path / file;
        fs::path destination = pack_path / file;
        fs::copy(source, destination, fs::copy_options::overwrite_existing);
    }
}
std::string removeFileExtension(const std::string &filename) {
    size_t last_dot = filename.find_last_of(".");
    if (last_dot == std::string::npos) {
        return filename; // 没有找到点，返回原始字符串
    }
    return filename.substr(0, last_dot);
}
std::string addDecrypt(const std::string &filename) {
    std::string name = removeFileExtension(filename);
    return name + "_decrypt" + filename.substr(filename.find_last_of("."));
}
std::string addDecompress(const std::string &filename) {
    std::string name = removeFileExtension(filename);
    return name + "_decompress" + filename.substr(filename.find_last_of("."));
}

// 获得目录下的文件信息
std::vector<FileInfo> getFiles(const std::string &directoryPath) {
    std::vector<FileInfo> files;
    for (const auto &entry : std::filesystem::directory_iterator(directoryPath)) {
        FileInfo fileInfo;
        fileInfo.name = entry.path().filename().string();
        fileInfo.isDirectory = entry.is_directory();
        fileInfo.size = entry.is_directory() ? 0 : entry.file_size();

        auto ftime = entry.last_write_time(); // 文件最后修改时间
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now()); // 转换为系统时钟时间
        std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
        fileInfo.lastModified = std::to_string(cftime);

        files.push_back(fileInfo);
        // std::cout << fileInfo.name << std::endl;
    }
    return files;
}

int main() {
    httplib::Server svr;
    std::string backendBasePath = "akcloud/akcloud-backend/";
    std::string backupBasePath = "backup/";
    std::string packBasePath = "pack/";
    std::string unpackBasePath = "unpack/";
    // 中间件：为所有响应添加 CORS 头
    svr.set_pre_routing_handler([](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        if (req.method == "OPTIONS") {
            res.status = 204; // No Content
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });

    svr.Options("/files", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("", "text/plain");
    });

    // 获得files
    svr.Get("/files", [](const httplib::Request &, httplib::Response &res) {
        std::cout << "Received request for /api/files" << std::endl;
        auto files = getFiles("./backup");
        nlohmann::json jsonFiles = nlohmann::json::array();
        for (const auto &file : files) {
            jsonFiles.push_back({{"name", file.name},
                                 {"isDirectory", file.isDirectory},
                                 {"size", file.size},
                                 {"lastModified", file.lastModified}});
        }
        res.set_content(jsonFiles.dump(), "application/json");
    });

    // 备份文件
    svr.Post("/files/backup", [&backupBasePath](const httplib::Request &req, httplib::Response &res) {
        std::cout << "Received request for /api/files/backup" << std::endl;
        try {
            auto file = req.get_file_value("file");
            auto relativePath = req.get_param_value("relativePath");
            std::string filePath = backupBasePath + "/" + file.filename;

            fs::create_directories(fs::path(filePath).parent_path());

            std::ofstream ofs(filePath, std::ios::binary);
            ofs.write(file.content.c_str(), file.content.size());
            ofs.close();
            res.status = 200;
            res.set_content("{\"message\": \"File uploaded successfully\"}", "application/json");
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid request\"}", "application/json");
        }
    });

    // 打包文件
    svr.Post("/files/pack", [&backupBasePath, &packBasePath](const httplib::Request &req, httplib::Response &res) {
        std::cout << "Received request for /api/files/pack" << std::endl;
        try {
            auto json = nlohmann::json::parse(req.body);
            std::vector<std::string> files = json["files"];
            std::string address = json["address"];

            fs::path pack_path = packBasePath;
            fs::path backup_path = backupBasePath;

            // 创建或清空 pack 文件夹
            if (fs::exists(pack_path)) {
                for (const auto &entry : fs::directory_iterator(pack_path))
                    fs::remove_all(entry.path());
            } else {
                fs::create_directory(pack_path);
            }
            // back→pack
            for (const auto &file : files) {
                fs::path source = backup_path / file;
                fs::path destination = pack_path / file;
                fs::copy(source, destination, fs::copy_options::overwrite_existing);
            }
            std::cout << address << std::endl;
            if (PackFile::packFile(pack_path.string(), address)) {
                res.status = 200;
                res.set_content("{\"message\": \"Files packed successfully\"}", "application/json");
            } else {
                res.status = 400;
                res.set_content("{\"error\": \"Failed to pack files\"}", "application/json");
            }

            res.status = 200;
            res.set_content("{\"message\": \"Files packed successfully\"}", "application/json");
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid request\"}", "application/json");
        }
    });

    // 解包文件
    svr.Post("/files/unpack", [&unpackBasePath, &backupBasePath](const httplib::Request &req, httplib::Response &res) {
        std::cout << "Received request for /api/files/unpack" << std::endl;
        try {
            auto file = req.get_file_value("file");
            auto address = req.get_file_value("address").content.c_str();
            std::string address2 = address;

            std::string filePath = unpackBasePath + "/" + file.filename;
            std::cout << "File path: " << filePath << std::endl;
            std::cout << "Address: " << address << std::endl;

            if (fs::exists(unpackBasePath)) {
                for (const auto &entry : fs::directory_iterator(unpackBasePath))
                    fs::remove_all(entry.path());
            } else {
                fs::create_directory(unpackBasePath);
            }
            fs::path unpack_path = unpackBasePath;
            fs::path destination = unpack_path / file.filename;

            std::ofstream ofs(destination, std::ios::binary);
            ofs.write(file.content.data(), file.content.size());
            ofs.close();
            std::cout << "Address: " << address2 << std::endl;

            if (PackFile::unpackFile(filePath, address2)) {
                res.status = 200;
                res.set_content("{\"message\": \"Files packed successfully\"}", "application/json");
            } else {
                res.status = 400;
                res.set_content("{\"error\": \"Failed to pack files\"}", "application/json");
            }

            res.status = 200;
            res.set_content("{\"message\": \"File uploaded successfully\"}", "application/json");
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid request\"}", "application/json");
        }
    });

    // 压缩文件
    svr.Post("/files/compress", [&unpackBasePath, &backupBasePath](const httplib::Request &req, httplib::Response &res) {
        std::cout << "Received request for /api/files/pack" << std::endl;
        try {
            auto json = nlohmann::json::parse(req.body);
            std::string file = json["files"];
            std::string method = json["method"];
            // 1 哈夫曼,2 LZ77

            std::string compress_path = backupBasePath + file;
            std::string destination_path = removeFileExtension(compress_path);
            int method_int = std::stoi(method);
            Composer composer(compress_path, destination_path);
            switch (method_int) {
            case 1:
                composer.startCompose();
                break;
            case 2:
                composer.compress_lz77();
                break;
            default:
                break;
            }
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid request\"}", "application/json");
        }
    });

    // 解压文件
    svr.Post("/files/decompress", [&unpackBasePath, &backupBasePath](const httplib::Request &req, httplib::Response &res) {
        std::cout << "Received request for /api/files/decompress" << std::endl;
        try {
            auto json = nlohmann::json::parse(req.body);
            std::string file = json["files"];

            std::string compress_path = backupBasePath + file;
            std::string destination_path = removeFileExtension(addDecompress(compress_path));
            std::cout << destination_path << std::endl;
            DeComposer decomposer(compress_path, destination_path);
            decomposer.decomposeAlgorithmSelector();
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid request\"}", "application/json");
        }
    });

    // 加密文件
    svr.Post("/files/encrypt", [&unpackBasePath, &backupBasePath](const httplib::Request &req, httplib::Response &res) {
        std::cout << "Received request for /api/files/encrypt" << std::endl;
        try {
            auto json = nlohmann::json::parse(req.body);
            std::string file = json["files"];
            std::string key = json["key"];

            std::string encrypt_path = backupBasePath + file;
            std::string destination_path = encrypt_path + ".aes";
            AESEncrypt aesEncrypt;
            aesEncrypt.encrypt(encrypt_path, destination_path, (unsigned char *)key.c_str());
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid request\"}", "application/json");
        }
    });

    // 解密文件
    svr.Post("/files/decrypt", [&unpackBasePath, &backupBasePath](const httplib::Request &req, httplib::Response &res) {
        std::cout << "Received request for /api/files/decrypt" << std::endl;
        try {
            auto json = nlohmann::json::parse(req.body);
            std::string file = json["files"];
            std::string key = json["key"];

            std::string encrypt_path = backupBasePath + file;
            std::string destination_path = addDecrypt(removeFileExtension(encrypt_path));
            AESEncrypt aesEncrypt;
            if (aesEncrypt.decrypt(encrypt_path, destination_path, (unsigned char *)key.c_str())) {
                res.status = 200;
                res.set_content("{\"message\": \"File decrypted successfully\"}", "application/json");
            } else {
                res.status = 400;
                res.set_content("{\"error\": \"Failed to decrypt file\"}", "application/json");
            }
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid request\"}", "application/json");
        }
    });

    std::cout << "Server is running on http://localhost:3001" << std::endl;
    if (!svr.listen("localhost", 3001)) {
        std::cerr << "Error starting server!" << std::endl;
        return 1;
    }

    return 0;
}