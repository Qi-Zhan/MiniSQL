#ifndef _RECORDMANAGER_H
#define _RECORDMANAGER_H
#include "head.h"
#include"BufferManager.h"
#include <iomanip>
/*每个文件夹 共几条 几块 每块多少条记录*/
class RecordManager{
public:
    bool createTable(std::string tableName,std::vector<int> colType);
    bool dropTable(std::string tableName);
    record insertRecord(std::string &tableName, std::vector<std::string>& tableValue, std::vector<int> &colType,std::vector<char> &isUnique);
    int deleteRecord(std::string tableName, std::vector<int> block_id, std::vector<int> offset, std::vector<int> colType); //利用索引删除
    int deleteRecord(std::string tableName, mycondition condition, std::vector<int> colOrder, std::vector<int> colType);   //硬删
    int deleteAll(std::string tableName, std::vector<int> colType);//直接重建表格即可
    int select(std::string &tableName, std::vector<int> &colType, std::vector<std::string> &colName, int mode, mycondition &condition, std::vector<int>& colOrder);
    //可能需要更多的mode来支持insert 无B+树Unique primary key 重复的判断 利用不同mode来完成代码的重用
private:
    int recordLength(std::vector<int>& colType);
    std::vector<std::string> getValue(char a[], std::vector<int> &colType);
    int deleteone(char block[], int offset,int length);
    bool satis(std::vector<std::string> &values, mycondition &condition, std::vector<int> &colOrder);
    bool initialHead(std::string &tableName, int &head, int &sumblock, std::vector<int> &blockRecord); //bool:0 empty
};
#endif