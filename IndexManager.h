#ifndef _INDEXMANAGER_H
#define _INDEXMANAGER_H
#include"head.h"
#include"BufferManager.h"
#include"BPlusTree.h"
class IndexManager{
public:
    bool createIndex(std::string indexName, std::string tableName, std::string colName);
    bool dropIndex(std::string indexName, std::string &tableName, std::string &colName);
    bool insertIndex(std::string indexName, std::string Key, int type,record pos);//type即float char int 驱动不同B+Tree
    std::vector<record> deleteRecord(std::string indexName, mycondition condition);
    record select(std::string indexName, mycondition condition);

private:
    std::string getColName(std::string indexName);
};
#endif