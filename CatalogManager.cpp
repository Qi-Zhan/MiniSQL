#include "CatalogManager.h"
using namespace std;
void CatalogManager::getTable(std::string tableName, std::vector<std::string> &colNow,int block_id){
    colNow.clear();
    int i = 5 + buffer.Page[block_id].pool[4];
    while (i < char4int(buffer.Page[block_id].pool)){
        int offset = buffer.Page[block_id].pool[i];
        string colName = getString(buffer.Page[block_id].pool + i + 1, offset);
        colNow.push_back(colName);
        if (buffer.Page[block_id].pool[i + offset + 3] == 'i')
            i = i + offset + 4 + 1 + buffer.Page[block_id].pool[i + offset + 4];
        else
            i = i + offset + 4;
    }
}
void CatalogManager::getTable(std::string tableName, std::vector<int> &typeNow,int block_id){
    typeNow.clear();
    int i = 5 + buffer.Page[block_id].pool[4];
    while (i < char4int(buffer.Page[block_id].pool)){
        int offset = buffer.Page[block_id].pool[i];
        // buffer.Page[block_id].prtpool();
        string colName = getString(buffer.Page[block_id].pool + i + 1, offset);
        // cout << colName << endl;
        if (buffer.Page[block_id].pool[i + 1 + offset] == '0'){
            if (buffer.Page[block_id].pool[i + 2 + offset] == '0'){
                typeNow.push_back(-1);
            }
            else
                typeNow.push_back(0);
        }
        else if (buffer.Page[block_id].pool[i  + 1 + offset] == '1'){
            typeNow.push_back(buffer.Page[block_id].pool[i + 2 + offset]);
        }
        else{
            // cout << "Wrong getTable" << endl;
            return;
        }
        if (buffer.Page[block_id].pool[i + offset + 3] == 'i')
            i = i + offset + 4 + 1 + buffer.Page[block_id].pool[i + offset + 4];
        else
            i = i + offset + 4;
    }
}
void CatalogManager::getTable(std::string tableName, std::vector<char> &isUnique, std::vector<std::string> &indexName, int block_id){
    isUnique.clear();
    int i = 5 + buffer.Page[block_id].pool[4];
    while (i < char4int(buffer.Page[block_id].pool)){
        int offset = buffer.Page[block_id].pool[i];
        isUnique.push_back(buffer.Page[block_id].pool[i + offset + 3]);
        if (buffer.Page[block_id].pool[i + offset + 3] == 'i'){
            indexName.push_back(getString(buffer.Page[block_id].pool + i + offset + 5, buffer.Page[block_id].pool[i + offset + 4]));
            i = i + offset + 4 + 1 + buffer.Page[block_id].pool[i + offset + 4];
        }else{
            if (buffer.Page[block_id].pool[i + offset + 3] == 'p'){
                indexName.push_back(tableName);
            }else
                indexName.push_back(" ");
                i = i + offset + 4;
        }
            
    }
}
int CatalogManager::existTable(std::string &tableName){
    // cout << "CatalogManager::existTable" << endl;
    string path ="dbfiles\\catalog\\" + tableName;
    FILE *pFile = fopen(path.data(), "rb");
    if(pFile==NULL)
        return -1;
    else{
        fclose(pFile);
        return buffer.inputBlock("catalog", tableName, 0);
    }
}
int CatalogManager::existTableCreate(std::string tableName){
    string path = "dbfiles\\catalog\\" + tableName;
    FILE *pFile = fopen(path.data(), "r");
    if (pFile == NULL)
        return -1;
    else{
        fclose(pFile);
        return 1;
    }
}
int CatalogManager::existIndex(std::string indexName, std::string tableName, std::string colName,int& table_id){
    int start = existTable(tableName);
    if (start == -1){
        return 0;
    }else{
        table_id = start;
        int i = 5 + buffer.Page[start].pool[4];
        while (i < char4int(buffer.Page[start].pool)){
            int offset = buffer.Page[start].pool[i];
            string colNow = getString(buffer.Page[start].pool + i + 1, offset);
            if (colNow == colName)
                return (i + offset + 3);
            if(buffer.Page[start].pool[i+offset+3]=='i') i = i + offset + 4+1+buffer.Page[start].pool[i+offset+4];
            else
                i = i + offset + 4;
        }
    }
    return -1;
}
bool CatalogManager::createTable(std::string tableName, std::vector<std::string> colName, std::vector<int> colType, std::vector<std::string> isUnique, std::string primary){
/*
    format: len(4)放总字符数.len(tablename)tablename{len(colname)colname+2bytes+1bytes}
            nexttable
    +n char(n)  0 int    -1 float 2字节存储  
    '1' 'n'    '0' '1'  '0'  '0'
    unique primary  index 1字节存储
    'u'    'p'      'i'     'n' 
    */
    if (existTableCreate(tableName) != -1){
        cout << "CatalogManager::Table '" << tableName << "' already exists" << endl;
        return 0;
    }
    char temp[BLOCK_SIZE + 1]; //将信息写入temp
    memset(temp, 0, BLOCK_SIZE);
    int point = 4;                    //记录当前指针和总字节数
    temp[point++] = tableName.size(); //len(tableName)
    for (int i = 0; i < tableName.size(); i++){ //tableName
        temp[point++] = tableName[i];
    }
    for (int i = 0; i < colName.size(); i++){
        // cout << colName[i] << endl;
        temp[point++] = colName[i].size(); //len(colName)
        for (int j = 0; j < colName[i].size(); j++){
            temp[point++] = colName[i][j]; //colName
            //printf("%c\n", temp[point - 1]);
        }
        if (colType[i] == 0){
            temp[point++] = '0';
            temp[point++] = '1';
        }
        else if (colType[i] == -1){
            temp[point++] = '0';
            temp[point++] = '0';
        }
        else{
            temp[point++] = '1';
            temp[point++] = char(colType[i] & 0xff);
        }
        if (primary.size() > 0 && colName[i] == primary){
            temp[point++] = 'p';
        }
        else if (isUnique[i] == "unique"){
            temp[point++] = 'u';
        }
        else{
            temp[point++] = 'n';
        }
        // cout << "gogogo" << endl;
    }
    intchar4(temp, point);
    string path = "dbfiles\\catalog\\" + tableName;
    FILE *pFile = fopen(path.data(), "wb");
    fclose(pFile);
    // cout << "cao" << endl;
    int table_id = buffer.inputBlock("catalog", tableName, 0);
    buffer.setdirty(table_id);
    memcpy(buffer.Page[table_id].pool, temp, point);
    // buffer.Page[table_id].prtpool();
    buffer.flush();
    return 1;
}
bool CatalogManager::createIndex(std::string indexName, std::string tableName, std::string colName){
    int table_id = 0;
    int point = existIndex(indexName, tableName, colName, table_id);
    if (point == 0){
        cout << "CatalogManager::Unknown table '" << tableName << "'" << endl;
        return 0;
    }
    else if (point == -1){
        cout << "CatalogManager::Unknown column '" << colName << "'" << endl;
        return 0;
    }
    else{
        if (buffer.Page[table_id].pool[point] == 'p' || buffer.Page[table_id].pool[point] == 'i'){
            cout << "CatalogManager::Index already exists" << endl;
            return 0;
        }
        else if (buffer.Page[table_id].pool[point] == 'n'){
            cout << "CatalogManager::Can't create index on '" << colName << "'" << endl;
            return 0;
        }else if (buffer.Page[table_id].pool[point] == 'u'){
            buffer.setdirty(table_id);
            // buffer.Page[table_id].prtpool();
            int allbyte = char4int(buffer.Page[table_id].pool);
            buffer.Page[table_id].pool[point] = 'i';
            for (int i = allbyte + 6; i > point; i--){
                buffer.Page[table_id].pool[i + indexName.size()+1] = buffer.Page[table_id].pool[i];
            }
            for (int i = 0; i < indexName.size(); i++){
                buffer.Page[table_id].pool[point + i+2] = indexName[i];
            }
            buffer.Page[table_id].pool[point + 1] = indexName.size();
            intchar4(buffer.Page[table_id].pool, allbyte + 1 + indexName.size());
            // buffer.Page[table_id].prtpool();
            buffer.flush();
            return 1;
        }else{
            cout << "CatalogManager::Wrong offset" << endl;
            return 0;
        }
    }
}
bool CatalogManager::dropIndex(std::string indexName, std::string tableName, std::string colName){
    int table_id;
    int start = existIndex(indexName, tableName, colName,table_id);
    if (buffer.Page[table_id].pool[start] == 'p'){
        cout << "CatalogManager::Can't drop index on primary key" << endl;
        return 0;
    }
    else{
        int allbyte = char4int(buffer.Page[table_id].pool);
        buffer.setdirty(table_id);
        buffer.Page[table_id].pool[start] = 'u';
        for (int i = start+1; i < allbyte+6; i++){
            buffer.Page[table_id].pool[i] = buffer.Page[table_id].pool[i + 1 + indexName.size()];
        }
        intchar4(buffer.Page[table_id].pool, allbyte - 1 - indexName.size());
        // buffer.Page[table_id].prtpool();
        buffer.flush();
    }
    return 1;
}
bool CatalogManager::dropTable(std::string tableName){
    int table_id = existTable(tableName);
    if (table_id==-1){
        cout << "CatalogManager::Unknown table '" << tableName << "'" << endl;
        return 0;
    }else{
        string path = "dbfiles\\catalog\\" + tableName;
        const char *a = path.data();
        if(remove(a)!=0){
            cout << "CatalogManager::remove table failed" << endl;
            return 0;
        }
        // buffer.clear("catalog", tableName, 0);
        return 1;
    }
}
bool CatalogManager::selectCheck(std::string tableName, std::vector<std::string> colName, std::vector<int> &colOrder, std::vector<std::string> &colAll){
    int point = existTable(tableName);
    if (point==-1){
        cout << "CatalogManager::Unknown table '" << tableName << "'" << endl;
        return 0;
    }
    else{
        vector<string> tableCol;
        getTable(tableName, tableCol,point);
        colAll = tableCol;
        for (int i = 0; i < colName.size(); i++){
            int j;
            for (j = 0; j < tableCol.size(); j++){
                if (colName[i] == tableCol[j]){
                    colOrder.push_back(j);
                    break;
                }
            }
            if (j == tableCol.size()){
                cout << "Unknown column '" << colName[i] << "'" << endl;
                return 0;
            }
        }
        return 1;
    }
}
bool CatalogManager::insertCheck(std::string &tableName, std::vector<int> &colType, std::vector<char> &isUnique, std::vector<std::string> &indexName){
    int point = existTable(tableName);
    if (point==-1){
        cout << "CatalogManager::Unknown table '" << tableName << "'" << endl;
        return 0;
    }else{
        // cout << "now" << endl;
        // buffer.Page[point].prtpool();
        vector<int> tableType;
        getTable(tableName, tableType,point);
        getTable(tableName, isUnique,indexName, point);
        if (tableType.size() != colType.size()){
            cout << "CatalogManager::Column count doesn't match value count" << endl;
            return 0;
        }
        for (int i = 0; i < tableType.size(); i++){
            if (tableType[i] != colType[i]){
                // cout << tableType[i] << " " << colType[i] << endl;
                cout << "CatalogManager::Column type doesn't match value type" << endl;
                return 0;
            }
        }
        // cout << "11" << endl;
        return 1;
    }
}
bool CatalogManager::selectall(std::string tableName, std::vector<std::string> &colAll,std::vector<int> &colType){
    int point = existTable(tableName);
    if (point==-1){
        cout << "CatalogManager::Unknown table '" << tableName << "'" << endl;
        return 0;
    }else{
        vector<string> tableCol;
        getTable(tableName, colAll,point);
        getTable(tableName, colType, point);
        return 1;
    }
}
bool CatalogManager::deleteall(std::string tableName){
    int point = existTable(tableName);
    if (point==-1){
        cout << "CatalogManager::Unknown table '" << tableName << "'" << endl;
        return 0;
    }
    return 1;
}