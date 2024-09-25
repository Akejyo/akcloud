#ifndef DECOMPOSER_H
#define DECOMPOSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
class huffmanTreeNode {
public:
    huffmanTreeNode(int nodevalue);
    huffmanTreeNode(int nodevalue, char cur);
    huffmanTreeNode *left;
    huffmanTreeNode *right;
    huffmanTreeNode *parent;
    int val;
    char c;
};
class DeComposer {
public:
    // 构造函数和析构函数
    DeComposer(const std::string &filename);
    ~DeComposer();

    void readFile();
    huffmanTreeNode *create_huffmanTree();
    void decompose(std::string outFilename);

private:
    std::ifstream infile; // 文件流
    std::string postFix;
    huffmanTreeNode *root;
    //源文件内容
    std::vector<std::string> lines;
    //字符数量统计
    std::unordered_map<unsigned char, int> charCount;
    // code
    std::unordered_map<char, std::string> strCode;
};

#endif //DECOMPOSER_H
