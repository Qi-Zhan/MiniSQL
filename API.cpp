#include"API.h"
using namespace std;
CatalogManager mycata;
RecordManager myrecord;
IndexManager myindex;
bool API::checkprimary(){
    if(primary.size()==0)
        return 1;//不存在 primary key
    for (int i = 0; i < condition.col.size(); i++){
        if(condition.col[i]==primary)
            return 1;
    }
    return 0;
}
void API::getins(instruction ins){
    this->ins = ins;
    if(ins.number==6 || ins.number==9){//需要condition
        int i = 1; 
        while (i<ins.info.size()){
            this->condition.col.push_back(ins.info[i]);
            i++;
            if(ins.info[i]=="=")
                this->condition.op.push_back(0);
            if (ins.info[i] == "<>")
                this->condition.op.push_back(1);
            if (ins.info[i] == "<")
                this->condition.op.push_back(2);
            if (ins.info[i] == ">")
                this->condition.op.push_back(3);
            if (ins.info[i] == "<=")
                this->condition.op.push_back(4);
            if (ins.info[i] == ">=")
                this->condition.op.push_back(5);
            i++;
            if (ins.info[i][0] == '\'' && ins.info[i][ins.info[i].size() - 1] == '\''){//检查是不是string
                ins.info[i] = ins.info[i].substr(1, ins.info[i].size() - 2);
                this->condition.type.push_back(ins.info[i].size());
            }else if(ins.info[i].find('.')!=ins.info[i].npos){
                this->condition.type.push_back(-1);
            }else//int
                this->condition.type.push_back(0);
            this->condition.value.push_back(ins.info[i]);
            i++;
        }
    }else if(this->ins.number==1){//create table col放
        int i = 1;
        while (i<this->ins.info.size()){
            if(ins.info[i]=="primary key"){
                primary = ins.info[i + 1];
                i = i + 2;
                if(i>=this->ins.info.size()){
                    break;
                }
            }else{
                this->condition.col.push_back(ins.info[i]);
                i++;
            }
            if(ins.info[i]=="char"){
                this->condition.type.push_back(stoi(ins.info[i + 1]));
                i = i + 2;
            }else if(ins.info[i]=="int"){
                this->condition.type.push_back(0);
                i++;
            }else{
                this->condition.type.push_back(-1);//float
                i++;
            }
            if (i >= this->ins.info.size()){
                this->condition.value.push_back("none");
                break;
            }
            if(ins.info[i]=="unique"){
                this->condition.value.push_back("unique");
                i++;
            }else{
                this->condition.value.push_back("none");
            }
            //cout << "i" << i << endl;
        }
        if(!checkprimary()){
            cout << "API::failed to set primary key" << endl;
            return;
        }
    }else if(ins.number==7){
        for (int i = 1; i < ins.info.size(); i++){
            if (ins.info[i][0] == '\'' && ins.info[i][ins.info[i].size() - 1] == '\''){ //检查是不是char
                ins.info[i] = ins.info[i].substr(1, ins.info[i].size() - 2);
                this->condition.type.push_back(ins.info[i].size());
            }
            else if (ins.info[i].find('.') != ins.info[i].npos){//float
                this->condition.type.push_back(-1);
            }
            else//int
                this->condition.type.push_back(0);
            this->condition.value.push_back(ins.info[i]);
        }
    }
    // for (int i = 0; i < this->condition.col.size(); i++){
    //     cout << this->condition.col[i]  << this->condition.value[i]<<this->condition.type[i] << endl;
    // }

}
void API::mainAPI(){
    if(ins.number==1){//create table
        if(mycata.createTable(ins.info[0], condition.col, condition.type, condition.value, primary)){
            if(primary.size()>0)
                myindex.createIndex( ins.info[0], ins.info[0], primary);//主键自动生成索引
            if (myrecord.createTable(ins.info[0], condition.type))
                cout << "API::Query OK" << endl;
        }
    }
    if (ins.number == 2){//drop table
        if(mycata.dropTable(ins.info[0])){
            if(myrecord.dropTable(ins.info[0]))
                cout << "API::Query OK" << endl;
        }
    }
    if (ins.number == 3){//create index
        if(mycata.createIndex(ins.info[0],ins.info[1],ins.info[2])){
            if (myindex.createIndex(ins.info[0], ins.info[1], ins.info[2]))
                cout << "API::Query OK" << endl;
        }   
    }
    if (ins.number == 4){//drop index
        string tableName, colName;
        if(myindex.dropIndex(ins.info[0],tableName,colName)){
            // cout << "tableName" << tableName << "colName" << colName << endl;
            if(mycata.dropIndex(ins.info[0],tableName,colName)){
                cout << "API::Query OK" << endl;
            }
        }
    }
    if (ins.number == 5){//select catalogmanager需要返回allcolumn
        vector<string> colAll;
        vector<int> colType;
        vector<int> colOrder;
        if(mycata.selectall(ins.info[0], colAll,colType)){
            // for (int i = 0; i < colAll.size(); i++)
            //     cout << colAll[i] << " " << endl;
            myrecord.select(ins.info[0], colType, colAll,0,condition,colOrder);
        }
    }
    if (ins.number == 6){//select condition
        vector<string> colAll;
        vector<int> colOrder;
        vector<int> colType;
        if(mycata.selectCheck(ins.info[0],condition.col,colOrder,colAll)){
            if (mycata.selectall(ins.info[0], colAll, colType)){
                myrecord.select(ins.info[0], colType, colAll, 1, condition, colOrder);
            }
        }
    }
    if (ins.number == 7){//insert
        vector<char> isUnique;
        vector<string> indexName;
        if (mycata.insertCheck(ins.info[0], condition.type,isUnique,indexName)){        
            record onerecord = myrecord.insertRecord(ins.info[0], condition.value, condition.type,isUnique);
            if(onerecord.block_id!=0 || onerecord.offset!=0){//维护索引  
                for (int i = 0; i < isUnique.size();i++){
                    // cout << "indexName"<<indexName[i] << endl;
                    if (isUnique[i]=='p' || isUnique[i]=='i'){
                        // cout << condition.value[i]<<condition.type[i]<<endl;
                        myindex.insertIndex(indexName[i], condition.value[i], condition.type[i], onerecord);
                    }
                }
                cout << "API::Query OK" << endl;
            }
        }
    }
    if (ins.number == 8){//delete
        vector<string> colAll;
        vector<int> colType;
        if (mycata.selectall(ins.info[0], colAll, colType)){
            // for (int i = 0; i < colAll.size(); i++)
            //     cout << colAll[i] << " " << endl;
            if(myrecord.deleteAll(ins.info[0], colType))
                cout << "API::Query OK" << endl;
        }
    }
    if (ins.number == 9){//delete condition
        vector<string> colAll;
        vector<int> colOrder;
        vector<int> colType;
        if (mycata.selectCheck(ins.info[0], condition.col, colOrder, colAll)){
            if (mycata.selectall(ins.info[0], colAll, colType)){
                myrecord.deleteRecord(ins.info[0], condition, colOrder, colType);
                cout << "API::Query OK" << endl;
            }
            // for (int i = 0; i < colOrder.size(); i++)
            // {
            //     cout << colOrder[i] << endl;
            // }
        }
    }
    this->condition.col.clear();
    this->condition.op.clear();
    this->condition.type.clear();
    this->condition.value.clear();
}
