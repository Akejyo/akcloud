#ifndef DECOMPOSER_H
#define DECOMPOSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
class huffmanTreeNode {
public:
    huffmanTreeNode(int nodevalue);
    huffmanTreeNode(int nodevalue, unsigned char cur);
    huffmanTreeNode *left;
    huffmanTreeNode *right;
    huffmanTreeNode *parent;
    int val;
    unsigned char c;
};
class DeComposer {
public:
    // 构造函数和析构函数
    DeComposer(const std::string &filename, const std::string &outputFilename);
    ~DeComposer();

    void decomposeAlgorithmSelector();

    void readFile();
    int readFileLz77();
    huffmanTreeNode *create_huffmanTree();
    void decompose(std::string outFilename);
    void startDecompose();
    int decompressLz77();
    bool readBit();

private:
    std::ifstream infile; // 文件流
    std::string postFix;
    std::string outputFilename;
    huffmanTreeNode *root;
    // 源文件内容
    std::vector<std::string> lines;
    // 字符数量统计
    std::map<unsigned char, int> charCount;
    // code
    std::map<unsigned char, std::string> strCode;
    // lz77相关
    std::vector<unsigned char> chars;
    // 读数据相关
    int bufferBits = 0;
    unsigned char readBuffer;
    // 相关令牌
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

#endif // DECOMPOSER_H