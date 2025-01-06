/*
解压缩测试文件
*/
#include "methods/composer.h"
#include "methods/decomposer.h"
int main() {
    //压缩指定路径下文件到指定输出文件
    Composer composer("D:\\mav\\testtxt.txt", "D:\\mav\\compressed");
     //composer.compress_lz77();

    composer.startCompose();
    //读取待解压文件,设置输出文件名（后缀不需要）
    DeComposer decomposer("D:\\mav\\compressed.hf", "D:\\mav\\decompressed");
    
    decomposer.decomposeAlgorithmSelector();

    return 0;
}