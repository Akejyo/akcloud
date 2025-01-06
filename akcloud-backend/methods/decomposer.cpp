#include "decomposer.h"
#include <queue>
#include <uchar.h>
#include <algorithm>
#include <cstring>
huffmanTreeNode::huffmanTreeNode(int nodevalue) {
    val = nodevalue;
    left = nullptr;
    right = nullptr;
    parent = nullptr;
};
huffmanTreeNode::huffmanTreeNode(int nodevalue, unsigned char cur) {
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
DeComposer::DeComposer(const std::string &filename, const std::string &outputFilename) {
    infile.open(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "无法打开文件: " << filename << std::endl;
    }
    this->outputFilename = outputFilename;
};
DeComposer::~DeComposer() {
    if (infile.is_open()) {
        infile.close();
    }
}
void DeComposer::readFile() {
    // 读取文件后缀名和频次行数信息

    std::getline(infile, postFix);

    postFix.erase(std::remove(postFix.begin(), postFix.end(), '\r'), postFix.end());
    std::string strLineCnt;
    std::getline(infile, strLineCnt);
    size_t lineCnt = atoi(strLineCnt.c_str());
    std::string lineContent = "";
    for (int i = 0; i < lineCnt; i++) {
        lineContent = "";
        std::getline(infile, lineContent);
        // 处理换行
        if (lineContent[0] == lineContent[1] && lineContent[1] == 'n') {
            charCount['\n'] = atoi(lineContent.c_str() + 3);
        } else if (lineContent[0] == lineContent[1] && lineContent[1] == 'r') {
            charCount['\r'] = atoi(lineContent.c_str() + 3);
        } else
            charCount[lineContent[0]] = atoi(lineContent.c_str() + 2);
    }
}
huffmanTreeNode *DeComposer::create_huffmanTree() {
    std::priority_queue<huffmanTreeNode *, std::vector<huffmanTreeNode *>, compare> pq;
    // 创建节点森林
    for (std::pair<unsigned char, int> p : charCount) {
        pq.push(new huffmanTreeNode(p.second, p.first));
    }
    // 构建
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
    outFilename += '.' + postFix;
    std::ofstream fOut(outFilename, std::ios::binary);
    huffmanTreeNode *cur = root;
    unsigned char bitcount = 0;
    // 文件大小，单位字节
    int fileSize = root->val;
    // 已解压字节数
    int compressSize = 0;
    unsigned char curch;
    while (1) {
        int i = infile.get();
        if (i == EOF) {
            break;
        }
        curch = static_cast<unsigned char>(i);
        bitcount = 0;
        while (bitcount < 8) {
            // 按照哈夫曼树进行遍历
            if (curch & 0x80) {
                cur = cur->right;
            } else {
                cur = cur->left;
            }
            // 遍历到底，代表解压一个字节
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
void DeComposer::decomposeAlgorithmSelector() {
    std::string post;
    std::getline(infile, post);
    if (post == ".hf") {
        startDecompose();
    } else {
        decompressLz77();
    }
}
void DeComposer::startDecompose() {
    // 读文件内容
    readFile();
    // 重建哈夫曼树
    create_huffmanTree();
    // 解压至outputFilename.xxx
    decompose(outputFilename);
}
bool DeComposer::readBit() {
    if (bufferBits == 0) {
        if (!infile.read(reinterpret_cast<char *>(&readBuffer), 1)) {
            return false; // EOF 或读取错误
        }
        bufferBits = 8; // 新读取一个字节，重置位数
    }
    bool bit = (readBuffer >> (bufferBits - 1)) & 1; // 提取最高位,或某一位
    bufferBits--;
    return bit;
}
int DeComposer::readFileLz77() {
    std::getline(infile, postFix);

    postFix.erase(std::remove(postFix.begin(), postFix.end(), '\r'), postFix.end());
    std::string strLineCnt;
    std::getline(infile, strLineCnt);
    int size = atoi(strLineCnt.c_str());
    return size;
}
int DeComposer::decompressLz77() {
    std::string compressed, window, buffer;
    int length = 0;
    // int cur_line = 0;
    // 读头,获取size
    int remaining = readFileLz77();
    window.resize(window_size);
    buffer.resize(buffer_size);

    int offset = 0;
    unsigned char next;

    // 根据头部信息大小设置ipos
    int ipos = 0;
    int opos = 0;
    int current_bit = 0;
    unsigned char curch;
    outputFilename += '.' + postFix;
    std::ofstream fOut(outputFilename, std::ios::binary);
    int i = 0;
    while (remaining > 0) {
        current_bit = readBit();
        ipos++;
        if (current_bit == 1) {
            // 不断读取bit,获取offset，length
            memset(&offset, 0, sizeof(int));
            for (int i = 0; i < window_bit; i++) {
                offset <<= 1;
                offset |= readBit();
                ipos++;
            }
            memset(&length, 0, sizeof(int));
            for (int i = 0; i < buffer_bit; i++) {
                length <<= 1;
                length |= readBit();
                ipos++;
            }
            next = 0x00;
            for (int i = 0; i < next_char_bit; i++) {
                next <<= 1;
                next |= readBit();
                ipos++;
            }
            i = 0;
            while (i < length && remaining > 0) {
                fOut << window[offset + i];
                opos++;
                buffer[i] = window[offset + i];
                i++;
                remaining--;
            }
            if (remaining > 0) {
                fOut << next;
                opos++;
                buffer[i] = next;
                remaining--;
            }
            length++;
        } else {
            next = 0x00;
            for (int i = 0; i < next_char_bit; i++) {
                next <<= 1;
                next |= readBit();
                ipos++;
            }
            fOut << next;
            opos++;
            if (remaining > 0) {
                buffer[0] = next;
            }
            remaining--;
            length = 1;
        }
        // 移动窗口和缓冲区
        window += window.substr(offset, length - 1);
        window += buffer[length - 1];
        window.erase(window.begin(), window.begin() + length);
    }
    return opos;
}
