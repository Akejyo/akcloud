#include "./methods/encrypt.h"
#include <iostream>

#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")

int main() {
    // 密钥和初始化向量，长度应符合 AES-256 要求
    unsigned char key[32] = {/* 32字节密钥 */
                             0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                             0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                             0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

    unsigned char ivec[16] = {/* 16字节初始化向量 */
                              0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
                              0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF};

    // 待加密文件名和输出文件名
    std::string inputFile = "D:\\mav\\plaintext.txt";
    std::string encryptedFile = "ciphertext.bin";
    std::string decryptedFile = "decrypted.txt";
    // std::ifstream ff("D:\\mav\\plaintext.txt");

    AESEncrypt aesEncrypt;
    //调用样例
    // 进行加密操作
    if (aesEncrypt.encrypt(inputFile, encryptedFile, key)) {
        std::cout << "加密成功，输出文件: " << encryptedFile << std::endl;
    } else {
        // std::cerr << "加密失败！" << std::endl;
        return 1;
    }

    // 进行解密操作
    if (aesEncrypt.decrypt(encryptedFile, decryptedFile, key)) {
        std::cout << "解密成功，输出文件: " << decryptedFile << std::endl;
    } else {
        // std::cerr << "解密失败！" << std::endl;
        return 1;
    }

    return 0;
}