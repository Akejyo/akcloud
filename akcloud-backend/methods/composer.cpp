#include "composer.h"
#include <queue>
#include <Uchar.h>
#include <algorithm>
huffmanNode::huffmanNode(int nodevalue) {
    val = nodevalue;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
}
huffmanNode::huffmanNode(int nodevalue, char cur) {
    val = nodevalue;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
    c = cur;
}
struct compare {
    bool operator()(huffmanNode *a, huffmanNode *b) {
        return a->val > b->val; // 定义最小堆*
    }
};
// 构造函数，尝试打开文件
Composer::Composer(const std::string &filename) {
    infile.open(filename);
    originFileName = filename;
    if (!infile) {
        std::cerr << "无法打开文件: " << filename << std::endl;
    }
}

Composer::~Composer() {
    if (infile.is_open()) {
        infile.close();
    }
}
//读取文件，统计每个字符数量
std::vector<std::string> Composer::readAllLines() {
    std::vector<std::string> clines;
    std::string line;
    while (std::getline(infile, line)) {
        clines.push_back(line); // 将每一行内容添加到 vector 中
    }
    lines = clines;
    for (std::string s : clines) {
        for (char c : s) {
            std::cout << c << ' ' << std::endl;
            if (!charCount.count(c))
                charCount[c] = 1;
            else
                charCount[c]++;
        }
    }
    return clines;
}
//构建哈夫曼树，返回树根
huffmanNode *Composer::create_huffmanTree() {
    std::priority_queue<huffmanNode *, std::vector<huffmanNode *>, compare> pq;
    //创建节点森林
    for (std::pair<char, int> p : charCount) {
        pq.push(new huffmanNode(p.second, p.first));
    }
    //构建
    while (pq.size() > 1) {
        huffmanNode *cleft = pq.top();
        pq.pop();
        huffmanNode *cright = pq.top();
        pq.pop();
        huffmanNode *parent = new huffmanNode(cleft->val + cright->val);
        parent->left = cleft;
        parent->right = cright;
        cleft->parent = parent;
        cright->parent = parent;
        pq.push(parent);
    }
    root = pq.top();
    return root;
}
//通过哈夫曼树获取哈夫曼编码
void Composer::generateHuffmanCode(huffmanNode *root) {
    if (root == nullptr) {
        return;
    }
    std::string code = "";
    if (root->left == nullptr && root->right == nullptr) {
        huffmanNode *cur = root;
        huffmanNode *parent = root->parent;
        char curchar = root->c;
        while (parent) {
            if (cur == parent->left) {
                code += '0';
            }
            if (cur == parent->right) {
                code += '1';
            }
            cur = parent;
            parent = cur->parent;
        }
        //将对应字符的编码储存进程序
        std::reverse(code.begin(), code.end());
        strCode[curchar] = code;
    }
    generateHuffmanCode(root->left);
    generateHuffmanCode(root->right);
}
void Composer::composerOutput(std::string outputFileName) {
    
    std::ofstream outfile(outputFileName); // 创建输出文件流
    writeHead(outfile,outputFileName);
    unsigned char ch = 0;
    unsigned char bitcount = 0;
    if (!outfile) {
        std::cerr << "无法创建输出文件: " << outputFileName << std::endl;
        return;
    }

    for (std::string line : lines) {
        for (char c : line) {
            std::string code = strCode[c];
            for (int i = 0; i < code.size(); i++) {
                ch <<= 1;
                if (code[i] == '1') {
                    ch |= 1;
                }
                bitcount++;
                if (bitcount == 8) {
                    bitcount = 0;
                    outfile << ch;
                }
            }
        }
    }
    //最后没有完整输出一个字节，特判
    if (bitcount > 0 && bitcount < 8) {
        ch <<= (8 - bitcount);
        outfile << ch;
    }
    outfile.close();
}
void Composer::writeHead(std::ostream &outfile,std::string filename) {
    //读取文件后缀名
    size_t pos = originFileName.find_last_of('.');
    std::string postFix = "";
    // 检查是否找到了 '.'，并且 '.' 不是最后一个字符
    if (pos != std::string::npos && pos != originFileName.length() - 1) {
        // 提取从 '.' 之后的子串（即文件后缀）
        postFix = originFileName.substr(pos + 1);
        //std::cout << "文件后缀名是: " << extension << std::endl;
    } else {
        //std::cout << "未找到有效的文件后缀名" << std::endl;
    }

   
    postFix += '\n';
    
    outfile << postFix;
    std::string info = "";
    size_t lineCnt = 0;
    //统计各字符出现个数，形成如A:1   B:3类似的数据
    for (std::pair<char, int> p : charCount) {
        info += p.first;
        info += ':';
        info += std::to_string(p.second);
        info += '\n';
        lineCnt++;
    }
    //文件总行数
    std::string totalLine = std::to_string(lineCnt);
    totalLine += '\n';

    outfile << totalLine << info;
   
}
