/*
解压缩测试文件
*/

#include "methods/composer.h"
#include "methods/decomposer.h"
int main() {
    //压缩指定路径下文件
    Composer composer("D:\\mav\\tt.txt");
    //读取文件内容
    composer.readAllLines();
    //构建哈夫曼树
    huffmanNode *root = composer.create_huffmanTree();
    //遍历树，获得哈夫曼编码
    composer.generateHuffmanCode(root);
    //替换源文件内容，压缩至out.zzz
    composer.composerOutput("out.zzz");
    //读取待解压文件
    DeComposer decomposer("out.zzz");
    //读文件内容
    decomposer.readFile();
    //重建哈夫曼树
    decomposer.create_huffmanTree();
    //解压至output.txt
    decomposer.decompose("output");
    return 0;
}