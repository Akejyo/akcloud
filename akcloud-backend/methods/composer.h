#ifndef FILEREADER_H
#define FILEREADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
class huffmanNode {
public:
    huffmanNode(int nodevalue);
    huffmanNode(int nodevalue, char cur);
    huffmanNode *left;
    huffmanNode *right;
    huffmanNode *parent;
    int val;
    char c;
};
class Composer {
public:
    // 构造函数和析构函数
    Composer(const std::string &filename);
    ~Composer();

    // 读取文件所有行并返回一个字符串的 vector
    std::vector<std::string> readAllLines();
    huffmanNode *create_huffmanTree();
    void generateHuffmanCode(huffmanNode *root);
    void composerOutput(std::string outputFileName);
    void writeHead(std::ostream &outfile, std::string filename);

private:
    std::ifstream infile; // 文件流
    std::string originFileName;
    huffmanNode *root;
    //源文件内容
    std::vector<std::string> lines;
    //字符数量统计
    std::unordered_map<char, int> charCount;
    // code
    std::unordered_map<char, std::string> strCode;
};

#endif // FILEREADER_H
