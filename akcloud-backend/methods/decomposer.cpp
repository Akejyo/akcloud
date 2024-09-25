#include "decomposer.h"
#include <queue>
#include <Uchar.h>
#include <algorithm>
huffmanTreeNode::huffmanTreeNode(int nodevalue) {
    val = nodevalue;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
};
huffmanTreeNode::huffmanTreeNode(int nodevalue, char cur) {
    val = nodevalue;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
    c = cur;
};
struct compare {
    bool operator()(huffmanTreeNode *a, huffmanTreeNode *b) {
        return a->val > b->val; // 定义最小堆*
    }
};
DeComposer::DeComposer(const std::string &filename) {
    infile.open(filename);
    if (!infile) {
        std::cerr << "无法打开文件: " << filename << std::endl;
    }
};
DeComposer::~DeComposer() {
    if (infile.is_open()) {
        infile.close();
    }
}
void DeComposer::readFile() {
    //读取文件后缀名和频次行数信息
    
    std::getline(infile, postFix);
    std::string strLineCnt;
    std::getline(infile, strLineCnt);
    size_t lineCnt = atoi(strLineCnt.c_str());
    std::string lineContent = "";
    for (int i = 0; i < lineCnt; i++) {
        lineContent = "";
        std::getline(infile, lineContent);
        charCount[lineContent[0]] = atoi(lineContent.c_str() + 2);
    }
}
huffmanTreeNode *DeComposer::create_huffmanTree() {
    std::priority_queue<huffmanTreeNode *, std::vector<huffmanTreeNode *>, compare> pq;
    //创建节点森林
    for (std::pair<char, int> p : charCount) {
        pq.push(new huffmanTreeNode(p.second, p.first));
    }
    //构建
    while (pq.size() > 1) {
        huffmanTreeNode *cleft = pq.top();
        pq.pop();
        huffmanTreeNode *cright = pq.top();
        pq.pop();
        huffmanTreeNode *parent = new huffmanTreeNode(cleft->val + cright->val);
        parent->left = cleft;
        parent->right = cright;
        cleft->parent = parent;
        cright->parent = parent;
        pq.push(parent);
    }
    root = pq.top();
    return root;
}
void DeComposer::decompose(std::string outFilename) {
    outFilename += '.'+postFix;
    std::ofstream fOut(outFilename);
    huffmanTreeNode *cur = root;
    unsigned char bitcount = 0;
    //文件大小，单位字节
    int fileSize = root->val;
    //已解压字节数
    int compressSize = 0;
    char curch;
    while (infile.get(curch)) {
        bitcount = 0;
        while (bitcount < 8) {
            //按照哈夫曼树进行遍历
            if (curch & 0x80) {
                cur = cur->right;
            } else {
                cur = cur->left;
            }
            //遍历到底，代表解压一个字节
            if (cur->left == nullptr && cur->right == nullptr) {
                fOut << cur->c;
                cur = root;
                compressSize++;
                if (compressSize == fileSize) {
                    break;
                }
            }
            bitcount++;
            curch <<= 1;
        }
    }
}
