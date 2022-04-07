#include"RecordManager.h"
using namespace std;
/*第0块表头：有几块4字节 一条记录长度4字节 每块多少记录4字节 可以算出是否满*/
/*数据表：以该数据表recordlength为一段一段分离 第一段为空闲链表头节点，都以n*recordlength形式 用前四个字节*/
int RecordManager::recordLength(std::vector<int> &colType){
    int length = 0;
    for (int i = 0; i < colType.size(); i++){
        if (colType[i] == -1 || colType[i] == 0)
            length += 4;
        else
            length += colType[i];
    }
    return length;
}
std::vector<std::string> RecordManager::getValue(char a[], std::vector<int> &colType){
    int pos = 0;
    vector<string> value;
    for (int i = 0; i < colType.size(); i++){
        if(colType[i]==0){//int
            value.push_back(to_string(char4int(a + pos)));
            pos += 4;
        }else if(colType[i]==-1){
            value.push_back(to_string(char4float(a + pos)));
            pos += 4;
        }else{//string
            value.push_back(getString(a + pos, colType[i]));
            pos += colType[i];
        }
    }
    // cout << "size" << colType.size() << endl;
    // cout << "4=" << value[3] << endl;
    return value;
}
bool RecordManager::initialHead(std::string &tableName, int &head, int &sumBlock, std::vector<int> &blockRecord){
    head = buffer.inputBlock("record", tableName, 0);
    sumBlock = char4int(buffer.Page[head].pool); //共有几块
    bool flag = 0;
    blockRecord.push_back(0);
    for (int i = 1; i <= sumBlock; i++){
        blockRecord.push_back(char4int(buffer.Page[head].pool + 4 * i + 4));
        if (blockRecord[i] != 0)
            flag = 1;
    }
    // for (int i = 1; i < blockRecord.size(); i++){
    //     cout << blockRecord[i] << endl;
    // }
    if (flag == 0) //空块
        return 0;
    return 1;
}
bool RecordManager::satis(std::vector<std::string> &values, mycondition &condition, std::vector<int> &colOrder){
    // cout << "colOrder size=" << colOrder.size() << endl;
    for (int i = 0; i < colOrder.size(); i++){
        // cout << "colOrder=" << colOrder[i] << endl;
        // cout << values[colOrder[i]] << endl;
        if(condition.type[i]==0){//int
            if (!condition.satify(stoi(values[colOrder[i]]), i))
                return 0;
        }
        else if (condition.type[i] == -1){//float
            if (!condition.satify(stof(values[colOrder[i]]), i))
                return 0;
        }else{
            if (!condition.satify(values[colOrder[i]], i))
                return 0;
        }
    }
    return 1;
}
int RecordManager::deleteone(char block[], int offset,int length){//即向链表中插入值
    int free = char4int(block);
    intchar4(block, offset);
    intchar4(block+offset, free);
    return 1;
}
bool RecordManager::createTable(std::string tableName, vector<int> colType){
    int Length = recordLength(colType);
    // cout << "recordLength=" << Length << endl;
    string Name = tableName;
    tableName = "dbfiles\\record\\" + tableName;
    const char *temp = tableName.data();
    FILE *pFile= fopen(temp, "wb");
    if(pFile!=NULL){
        char kong[9];
        memset(kong, 0, 4);
        intchar4(kong + 4, Length);
        fwrite(kong, 8, 1, pFile);
        fclose(pFile);
        return 1;
    }else{
        cout << "RecordManager::fail to createTable '" << Name << "'" << endl;
        return 0;
    }
}
bool RecordManager::dropTable(std::string tableName){
    string Name = tableName;
    tableName = "dbfiles\\record\\" + tableName;
    const char *temp = tableName.data();
    if (remove(temp)==0){
        return 1;
    }
    else{
        cout << "RecordManager::fail to dropTable '" << Name << "'" << endl;
        return 0;
    }
}
record RecordManager::insertRecord(std::string &tableName, std::vector<std::string> &tableValue, std::vector<int> &colType, std::vector<char> &isUnique){
    mycondition useless;
    vector<int> pain;
    for (int i = 0; i < isUnique.size(); i++){
        pain.push_back(isUnique[i] == 'n' ? 0 : 1);
    }
    if(!select(tableName,colType,tableValue,2,useless,pain)){
        record useless2(0,0);
        return useless2;
    }
    
    int Length = recordLength(colType);//一条记录的长度
    int head = buffer.inputBlock("record", tableName, 0);//第0块
    buffer.Page[head].setpin(1);//锁住防止被替换
    int sumBlock = char4int(buffer.Page[head].pool);//共有几块
    // cout << "sumBlock=" << sumBlock << endl;
    int firstBlock = 1; //第一个可以插入的块
    int sumrecord;//该块总记录数
    for (; firstBlock <= sumBlock; firstBlock++){
        sumrecord = char4int(buffer.Page[head].pool + 4 + 4 * firstBlock);
        // cout << "sumrecord=" << sumrecord << endl;
        if(sumrecord<=BLOCK_SIZE/Length-4)
            break;
    }
    int block_id = buffer.inputBlock("record", tableName, firstBlock);
    buffer.Page[block_id].setpin(1);
    int pos = 0;//插入具体位置
    if(sumBlock<firstBlock){//要开一个新的块
        // cout << "newblock" << endl;
        intchar4(buffer.Page[head].pool, sumBlock + 1);//总块数++
        intchar4(buffer.Page[head].pool + 4+firstBlock * 4, 1);//新块记录数为1
        buffer.setdirty(head);//脏块
        intchar4(buffer.Page[block_id].pool, 2*Length);//空闲链表指向第二条记录,即2*length为下次插入地方
        pos = Length;
    }else{
        intchar4(buffer.Page[head].pool + 4 + firstBlock * 4, sumrecord+1);
        buffer.setdirty(head);
        pos = char4int(buffer.Page[block_id].pool);//找到第一个空闲的位置
        // cout <<"blockid="<<block_id<< "pos" <<pos<< endl;
        if(pos/Length<=sumrecord){//这不是最后一个空闲位置
            int nextpos = char4int(buffer.Page[block_id].pool + pos);
            intchar4(buffer.Page[block_id].pool, nextpos);
        }else{
            // cout << "lastpostion" << endl;
            // cout << "pos+length" << pos + Length << endl;
            intchar4(buffer.Page[block_id].pool, pos + Length);
            // cout << "pos+length" << char4int(buffer.Page[block_id].pool) << endl;
        }
    }
    for (int i = 0; i < tableValue.size(); i++){
        if(colType[i]==0){//int
            intchar4(buffer.Page[block_id].pool + pos, stoi(tableValue[i]));
            pos += 4;
        }else if(colType[i]==-1){
            // cout << "what you insert is " << tableValue[i] << endl;
            floatchar4(buffer.Page[block_id].pool + pos, stof(tableValue[i]));
            pos += 4;
        }else{
            for (int j = 0; j < tableValue[i].size(); j++){
                buffer.Page[block_id].pool[pos++] = tableValue[i][j];
            }
        }
    }
    buffer.setdirty(block_id);
    buffer.flush();
    buffer.Page[head].setpin(0);
    buffer.Page[block_id].setpin(0);
    record onerecord(firstBlock,pos/Length-1);
    return onerecord;
}
int RecordManager::deleteRecord(std::string tableName, std::vector<int> block_id, std::vector<int> offset, std::vector<int> colType){
    int Length = recordLength(colType);
    // cout << "deleteRecord::length" << Length << endl;
    int head, sumBlock;
    vector<int> blockRecord;
    if (initialHead(tableName, head, sumBlock, blockRecord) == 0) //empty set
        return 1;
    vector<int> table_id;//还要修改head
    for (int i = 0; i < block_id.size(); i++){
        table_id.push_back(buffer.inputBlock("record", tableName, block_id[i]));
        deleteone(buffer.Page[table_id[i]].pool, offset[i],recordLength(colType));
        buffer.setdirty(table_id[i]);
        buffer.Page[table_id[i]].setpin(1);
    }
    buffer.flush();
    for (int i = 0; i < table_id.size(); i++){
        buffer.Page[table_id[i]].setpin(0);
    }
    buffer.Page[head].setpin(0);
    return 1;
} //利用索引删除
int RecordManager::deleteRecord(std::string tableName, mycondition condition, std::vector<int> colOrder, std::vector<int> colType){
    int Length = recordLength(colType);
    int head, sumBlock;
    vector<int> blockRecord;
    if(initialHead(tableName,head,sumBlock,blockRecord)==0)//empty set
        return 1;
    vector<int> block_id;
    block_id.push_back(0);
    int  nowrecord, pos, free, nextfree; 
    //free:空闲链表所在位置 pos:当前指向位置 nowrecord:当前总记录数
    vector<string> out;
    for (int i = 1; i <=sumBlock; i++){
        // cout << "blockrecord=" << blockRecord[i] << endl;
        block_id.push_back(buffer.inputBlock("record", tableName, i));
        buffer.Page[block_id[i]].setpin(1);//pin住该块
        nowrecord = 0;
        free = 0;
        nextfree = char4int(buffer.Page[block_id[i]].pool + free);
        while (nextfree == free + Length){ //找到第一个记录位置
            free = nextfree;
            nextfree = char4int(buffer.Page[block_id[i]].pool + nextfree);
        }
        pos = free + Length;
        int afterrecord = blockRecord[i];
        while (nowrecord < blockRecord[i]){
            if (free == pos){
                pos += Length;
                free = char4int(buffer.Page[block_id[i]].pool + free);
            }else{
                // cout << "afterrecord" << afterrecord << endl;
                out = getValue(buffer.Page[block_id[i]].pool + pos, colType);
                if(satis(out,condition,colOrder)){//满足条件则删除
                    deleteone(buffer.Page[block_id[i]].pool, pos, Length);
                    afterrecord--;
                }
                nowrecord++;
                pos += Length;
            }
        }
        if(afterrecord<blockRecord[i]){
            intchar4(buffer.Page[head].pool + 4 + 4 * i, afterrecord);
            buffer.setdirty(block_id[i]);
            buffer.setdirty(head);
            // cout << "afterrecord" << afterrecord << endl;
        }
    }
    buffer.flush();
    for (int i = 0; i < blockRecord.size(); i++){
        buffer.Page[block_id[i]].setpin(0);
    }
    buffer.Page[head].setpin(0);
    return 1;
} //硬删
int RecordManager::deleteAll(std::string tableName,std::vector<int> colType){
    return (createTable(tableName,colType));
}
int RecordManager::select(std::string &tableName, std::vector<int> &colType, std::vector<std::string> &colName, int mode, mycondition& condition, std::vector<int> &colOrder){
    /*mode 0: select all mode 1: select condition mode 2: insertcheck */
    int head, sumBlock;
    vector<int> blockRecord;
    if(initialHead(tableName,head,sumBlock,blockRecord)==0&&mode<2){
        cout << "empty set" << endl;
        return 0;
    }
    int Length = recordLength(colType);
    int block_id, nowrecord, pos, free,nextfree;//free:空闲链表所在位置 pos:当前指向位置 nowrecord:当前总记录数
    vector<string> out;
    int flag = 0; //判断 empty set
    for (int i = 1; i <= sumBlock; i++){
        // cout << "select blockrecord"<<i<<" = " << blockRecord[i] << endl;
        block_id = buffer.inputBlock("record", tableName, i);
        buffer.Page[block_id].setpin(1);
        nowrecord = 0;
        free = 0;
        nextfree = char4int(buffer.Page[block_id].pool + free);
        while (nextfree==free+Length){//找到第一个记录位置
            free = nextfree;
            nextfree = char4int(buffer.Page[block_id].pool + nextfree);
        }
        pos = free + Length;
        // cout << "selectpos" << pos << endl;
        while (nowrecord<blockRecord[i]){
            if(nextfree==pos){
                pos += Length;
                nextfree = char4int(buffer.Page[block_id].pool + nextfree);
            }else{
                out = getValue(buffer.Page[block_id].pool + pos, colType);
                if (mode == 0 || ((mode == 1) && satis(out, condition, colOrder))){
                    flag++;
                    if (flag == 1){
                        for (int i = 0; i < colName.size(); i++){
                            if(colType[i]<=0) cout <<left<<  setw(6) << colName[i];
                            else
                                cout <<left<< setw(colType[i]+5) << colName[i];
                        }
                        cout << endl;
                        //输出colALL
                    }
                    for (int i = 0; i < out.size(); i++){
                        if (colType[i] <= 0)
                            cout << left<< setw(6) << out[i];
                        else
                            cout << left<< setw(colType[i]+5) << out[i];
                    }
                    cout << endl;
                }else if(mode==2){//mode = 3 dupli
                      // Duplicate entry 'wy' for key 'sname'
                    for (int i = 0; i < colOrder.size(); i++){
                        if(colOrder[i]==1 && colName[i]==out[i]){
                            cout << "Duplicate entry '" << out[i] <<"'"<< endl;
                            return 0;
                        }   
                    }
                }
                nowrecord++;
                pos += Length;
            }
        }
        buffer.Page[block_id].setpin(0);
    }
    if(flag==0 && mode<2)
        cout << "empty set" << endl;
    return 1;
}