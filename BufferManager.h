#ifndef _BUFFERMANAGER_H
#define _BUFFERMANAGER_H
#include"head.h"
#include<list>
#include<map>
#define BLOCK_SIZE 4096
#define BLOCK_NUM 4096
class Block{//用block来表述一个块
public:
    void setpin(int i) { pin = i; };
    int getpin() { return pin; };
    char pool[BLOCK_SIZE];
    void prtpool(); //调试
    bool exist(std::string content, std::string fileName, int block_id);
    void setFile(std::string content, std::string fileName,int block_id);
    bool inputFile(std::string content, std::string fileName, int block_id);
    bool outputFile(std::string content, std::string fileName, int block_id);
    bool outputFile();
private:
    std::string fileName;
    std::string content;
    std::string path;
    int pin;//锁住
    int block_id;
};
class LRU{
public:
    LRU();
    int insert(std::string path);
    int exist(std::string path) { return (myhash.find(path) == myhash.end()) ? -1 : *myhash.find(path)->second; };
    void test();
private:
    std::map<std::string, std::list<int>::iterator> myhash;
    std::list<int> mylist;
    int free;
};
class BufferManager{//提供一个装block的池并负责与其他模块交互,其他模块要清楚该拿哪个以及里面存了什么
public:
    BufferManager();
    Block Page[BLOCK_NUM];
    int inputBlock(std::string content, std::string fileName, int block_id);
    int findBlock(std::string content, std::string fileName,int block_id);//找到一个要写到指定目录的块
    int output(int buffer_id, std::string path, int block_id);
    void setdirty(int i) { dirty.emplace_back(i);};
    void clear(std::string content, std::string fileName, int block_id);
    int flush();
private:
    LRU myLRU; //用于缓冲区替换策略
    int existBlock(std::string content, std::string fileName, int block_id);
    std::vector<int> dirty;
};
extern BufferManager buffer;
#endif
