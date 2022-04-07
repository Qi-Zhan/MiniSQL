#include"IndexManager.h"
using namespace std;

bool IndexManager::createIndex(std::string indexName, std::string tableName, std::string colName){
    //初始化第0块和第一块即根结点
    string Name = indexName;
    string mypath = "dbfiles\\index\\" + indexName;
    const char *temp = mypath.data();
    FILE *pFile = fopen(temp, "wb");
    if (pFile != NULL){
        char firstblock[BLOCK_SIZE];
        int point = 0;
        firstblock[point++] = tableName.size();
        for (int i = 0; i < tableName.size(); i++){
            firstblock[point++] = tableName[i];
        }
        firstblock[point++] = colName.size();
        for (int i = 0; i < colName.size(); i++){
            firstblock[point++] = colName[i];
        }
        intchar4(firstblock+point, 2);//第一个空闲块block_id = 2
        fwrite(firstblock, BLOCK_SIZE, 1, pFile);//初始化第0块        
        memset(firstblock, 0, BLOCK_SIZE);
        firstblock[0] = 'L';
        fseek(pFile, BLOCK_SIZE, 0);
        fwrite(firstblock, BLOCK_SIZE, 1, pFile);//初始化第1块
        fclose(pFile);
        return 1;
    }
    else{
        cout << "IndexManager::fail to createIndex '" << Name << "'" << endl;
        return 0;
    }
}
bool IndexManager::dropIndex(std::string indexName,std::string &tableName, std::string &colName){
    string Name = indexName;
    indexName = "dbfiles\\index\\" + indexName;
    const char *temp = indexName.data();
    FILE *pFile = fopen(temp, "rb");
    if(pFile==NULL){
        cout << "IndexManager::No such index '" << Name << "'" << endl;
        return 0;
    }
    // cout << "1" << endl;
    char firstblock[BLOCK_SIZE];
    fread(firstblock, BLOCK_SIZE, 1, pFile);
    int len = firstblock[0];
    tableName = getString(firstblock + 1, len);
    int len2 = firstblock[1 + len];
    colName = getString(firstblock +len+2, len2);
    fclose(pFile);
    if (remove(temp) == 0){
        return 1;
    }else{
        cout << "IndexManager::fail to dropIndex '" << Name << "'" << endl;
        return 0;
    }
}
bool IndexManager::insertIndex(std::string indexName, std::string Key, int type, record pos){
    BPlusTree myTree;
    myTree.initialize(indexName, "", type);
    myTree.insertkey(Key, pos);
    return 1;
} //type即float char int 
std::vector<record> IndexManager::deleteRecord(std::string indexName, mycondition condition){
    vector<record> needDelete;
    string colName = getColName(indexName);
    mycondition rcondition = condition.project(colName);
    return needDelete;
}
record IndexManager::select(std::string indexName, mycondition condition){
    string colName = getColName(indexName);
    mycondition rcondition = condition.project(colName);
    BPlusTree myTree;
    myTree.initialize(indexName, "", condition.op[0]);
    return myTree.findKey(condition.value[0], 1);//root 为第一块
}
std::string IndexManager::getColName(std::string indexName){
    int head = buffer.inputBlock("record", indexName, 0);
    int len = buffer.Page[head].pool[0];
    int len2 = buffer.Page[head].pool[1 + len];
    return getString(buffer.Page[head].pool + len + 2, len2);
}