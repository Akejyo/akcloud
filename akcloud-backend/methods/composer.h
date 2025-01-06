#ifndef FILEREADER_H
#define FILEREADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
class huffmanNode {
public:
    huffmanNode(int nodevalue);
    huffmanNode(int nodevalue, unsigned char cur);
    huffmanNode *left;
    huffmanNode *right;
    huffmanNode *parent;
    int val;
    unsigned char c;
};
class Composer {
public:
    // 构造函数和析构函数
    Composer(const std::string &filename, const std::string &outputFilename);
    ~Composer();

    // 读取文件所有行并返回一个字符串的 vector
    std::vector<unsigned char> readAllLines();
    huffmanNode *create_huffmanTree();
    void generateHuffmanCode(huffmanNode *root);
    void composerOutput(std::string outputFileName);
    void writeHead(std::ostream &outfile, std::string filename);
    void startCompose();

    // lz77压缩
    int compress_lz77();
    int match(std::string &window, std::string &buffer, int *offset, unsigned char *next);

private:
    std::ifstream infile; // 文件流

    std::string originFileName;
    std::string outputFilename;
    huffmanNode *root;
    // 源文件内容
    std::vector<std::string> lines;
    std::vector<unsigned char> chars;
    // 字符数量统计
    std::map<unsigned char, int> charCount;
    // code
    std::map<unsigned char, std::string> strCode;

    // lz77
    int window_size = 4096;
    int buffer_size = 32;
    int window_bit = 12;
    int buffer_bit = 5;
    int next_char_bit = 8;
    // 找到匹配串时的token,26位
    int token_bit = 26;
    // 未找到匹配串，9位
    int unfound_token_bit = 9;
};

#endif // FILEREADER_H