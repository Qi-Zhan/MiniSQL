#ifndef _CATALOGMANAGER_H
#define _CATALOGMANAGER_H 
#include"head.h"
#include "BufferManager.h"
class CatalogManager{
public:
    bool createTable(std::string tableName, std::vector<std::string> colName, std::vector<int> colType, std::vector<std::string> isUnique,std::string primary);
    bool createIndex(std::string indexName, std::string tableName,std::string colName);
    bool dropTable(std::string tableName);
    bool dropIndex(std::string indexName, std::string tableName, std::string colName);
    bool selectCheck(std::string tableName, std::vector<std::string> colName,std::vector<int> &colOrder,std::vector<std::string> &colAll);//select or delete check
    bool insertCheck(std::string &tableName, std::vector<int> &colType,std::vector<char>&isUnique, std::vector<std::string>&indexName);
    bool selectall(std::string tableName,std::vector<std::string>&colAll,std::vector<int>&colType);//select delete all check
    bool deleteall(std::string tableName);
private:
    int existTableCreate(std::string tableName);
    void getTable(std::string tableName, std::vector<std::string> &colNow,int block_id);//得到每一列的名字
    void getTable(std::string tableName, std::vector<int> &typeNow,int block_id);//得到每一列所存储的类型
    void getTable(std::string tableName, std::vector<char> &isUnique, std::vector<std::string> &indexName,int block_id); //得到每一列是否unique
    int existTable(std::string &tableName);
    int existIndex(std::string indexName, std::string tableName, std::string colName,int &table_id);
    
};
#endif