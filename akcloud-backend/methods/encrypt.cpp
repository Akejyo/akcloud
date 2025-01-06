#include "encrypt.h"
AESEncrypt::AESEncrypt() {
    ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
    ctx1 = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx1);
}
AESEncrypt::~AESEncrypt() {
    // 释放CTX
    EVP_CIPHER_CTX_cleanup(ctx);
    EVP_CIPHER_CTX_free(ctx);
    EVP_CIPHER_CTX_cleanup(ctx1);
    EVP_CIPHER_CTX_free(ctx1);
}
bool AESEncrypt::encrypt(const std::string &in, std::string &out, const unsigned char *key) {
    unsigned char *ivec = generateIV(16);
    for (size_t i = 0; i < 16; ++i) {
        std::cout << std::hex << (int)ivec[i] << " "; // 以十六进制显示IV
    }

    bool ret = 0;
    std::ifstream fIn(in, std::ios::in | std::ios::binary);
    if (!fIn.is_open()) {
        std::cerr << "Error: Failed to open file " << std::endl;
        return 0;
    }
    std::ofstream fOut(out, std::ios::out | std::ios::binary);
    if (!fOut.is_open()) {
        std::cerr << "Error: Failed to open file " << std::endl;
        return 0;
    }
    fOut.write(reinterpret_cast<char *>(ivec), 16);
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, ivec);

    fIn.seekg(0, std::ios::end);
    uint64_t inputFileLen = fIn.tellg();
    fIn.seekg(0, std::ios::beg);

    char readBuf[8192] = {0x00};
    //使用unsigned char处理字节，写入时需要强制转换为char
    uint8_t writeBuf[8192 + 32] = {0x00};

    uint64_t Total = 0;
    while (Total < inputFileLen) {
        int curRead = sizeof(readBuf);
        if (inputFileLen - Total < sizeof(readBuf)) {
            curRead = inputFileLen - Total;
        }
        fIn.read(readBuf, curRead);
        Total = Total + curRead;
        int toEnBufLen = 0;
        int curEnLen = 0;
        if (!EVP_EncryptUpdate(ctx, writeBuf, &toEnBufLen, reinterpret_cast<const unsigned char *>(readBuf), curRead)) {
            return ret;
        }
        //分块加密，如果是最后一块,调用结束,获取填充数据
        if (curRead < sizeof(readBuf)) {
            if (!EVP_EncryptFinal_ex(ctx, writeBuf + toEnBufLen, &curEnLen)) {
                return ret;
            };
            toEnBufLen += curEnLen;
        }
        fOut.write(reinterpret_cast<const char *>(writeBuf), toEnBufLen);
    }
    fIn.close();
    fOut.close();
    ret = 1;
    return ret;
}
bool AESEncrypt::decrypt(const std::string &in, std::string &out, const unsigned char *key) {
    std::ifstream fIn(in, std::ios::in | std::ios::binary);
    int rr = 1;
    std::ofstream fOut(out, std::ios::out | std::ios::binary);
    if (!fIn.is_open()) {
        std::cerr << "Error: Failed to open file " << std::endl;
        return 0;
    }

    if (!fOut.is_open()) {
        std::cerr << "Error: Failed to open file " << std::endl;
        return 0;
    }

    unsigned char *ivec = new unsigned char[16];

    fIn.seekg(0, std::ios::end);
    uint64_t inputFileLen = fIn.tellg();
    fIn.seekg(0, std::ios::beg);
    inputFileLen -= 16;
    fIn.read(reinterpret_cast<char *>(ivec), 16);
    rr = EVP_DecryptInit_ex(ctx1, EVP_aes_256_cbc(), NULL, key, ivec);
    char readBuf[8192] = {0x00};

    uint8_t writeBuf[8192 + 32] = {0x00};
    uint64_t Total = 0;
    while (Total < inputFileLen) {
        int curRead = sizeof(readBuf);
        if (inputFileLen - Total < sizeof(readBuf)) {
            curRead = inputFileLen - Total;
        }
        fIn.read(readBuf, curRead);
        Total += curRead;
        int toEnBufLen = 0;
        int curEnLen = 0;
        if (!EVP_DecryptUpdate(ctx1, writeBuf, &toEnBufLen, reinterpret_cast<const unsigned char *>(readBuf), curRead)) {
            printf("EVP_DecryptUpdate failed!  err:%s  \n", ERR_error_string(ERR_get_error(), NULL));
            return false;
        }

        if (curRead < sizeof(readBuf)) {
            if (!EVP_DecryptFinal(ctx1, writeBuf + toEnBufLen, &curEnLen)) {
                printf("EVP_DecryptFinal failed!  err:%s  \n", ERR_error_string(ERR_get_error(), NULL));
                return false;
            }
            toEnBufLen += curEnLen;
        }
        fOut.write(reinterpret_cast<const char *>(writeBuf), toEnBufLen);
    }
    fIn.close();
    fOut.close();
    return rr;
}

unsigned char *AESEncrypt::generateIV(size_t length) {
    unsigned char *iv = new unsigned char[length];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255);

    for (size_t i = 0; i < length; ++i) {
        iv[i] = static_cast<unsigned char>(dis(gen));
    }

    return iv;
}