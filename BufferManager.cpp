#include"BufferManager.h"
using namespace std;
BufferManager buffer;
void Block::prtpool(){
    for (int i = 0; i < 100; i++)
        printf("%d ", pool[i]);
    printf("\n");
}
void Block::setFile(std::string content, std::string fileName, int block_id){
    this->content = content;
    this->fileName = fileName;
    this->block_id = block_id;
    this->path = "dbfiles\\" + content + "\\" + fileName;
} 
bool Block::exist(std::string content, std::string fileName, int block_id){//缓冲区找
    if(content==this->content && fileName==this->fileName && block_id==this->block_id)
        return 1;
    return 0;
}
bool Block::inputFile(std::string content, std::string fileName, int block_id){//从硬盘读入文件
    setFile(content, fileName,block_id);
    FILE* pFile = fopen(path.data(), "rb");
    if(pFile==NULL)
        return 0;
    fseek(pFile, block_id * BLOCK_SIZE, SEEK_SET);
    fread(this->pool, BLOCK_SIZE, 1, pFile);
    fclose(pFile);
    return 1;
}
bool Block::outputFile(std::string Content, std::string FileName, int block_Id){
    FILE *pFile = fopen(path.data(), "rb+");
    if(pFile==nullptr){
        fclose(pFile);
        FILE *pFile = fopen(path.data(), "wb+");
    }
    fseek(pFile, block_Id * BLOCK_SIZE, SEEK_SET);
    fwrite(pool, BLOCK_SIZE, 1, pFile);
    fclose(pFile);
    return 1;
}
bool Block::outputFile(){
    FILE *pFile = fopen(path.data(), "rb+");
    if (pFile == NULL){
        // cout << "need to write" << endl;
        fclose(pFile);
        pFile = fopen(path.data(), "wb+");
        fclose(pFile);
        pFile = fopen(path.data(), "rb+");
    }
    // cout << "outputfile" << path << endl;
    fseek(pFile, block_id * BLOCK_SIZE, SEEK_SET);
    fwrite(pool, BLOCK_SIZE, 1, pFile);
    fclose(pFile);
    return 1;
}
LRU::LRU(){
    free = 0;
    mylist.clear();
    myhash.clear();
}
int LRU::insert(string path){
    if (myhash.find(path) != myhash.end()){ //该块已经在buffer中了
        list<int>::iterator iter = myhash.find(path)->second;
        int x = *iter;
        mylist.remove(x);
        // cout << "delete iter" << endl;
        // test();
        mylist.push_front(x);
        myhash[path] = mylist.begin();
        // cout <<"already exist " <<path << "  "<<*myhash.find(path)->second << endl;
        return *mylist.begin();
    }
    else if (free < BLOCK_NUM){ //有空余的位置
        mylist.push_front(free++);
        myhash[path] = mylist.begin();
        // cout << "exist free "<<path << *myhash.find(path)->second << endl;
        return free - 1;
    }else{ //替换最后一个
        map<string, list<int>::iterator>::iterator iter = myhash.begin();
        int x = *prev(mylist.end());
        for (; iter != myhash.end();){
            if (*iter->second == x)
                myhash.erase(iter++);
            else
                iter++;
        }
        mylist.erase(prev(mylist.end()));
        mylist.push_front(x);
        myhash[path] = mylist.begin();
        return *mylist.begin();
    }
    
}
void LRU::test(){
    cout << "free=" << free << endl;
    list<int>::iterator iter = mylist.begin();
    for (; iter != mylist.end(); iter++){
        cout << *iter << " ";
    }
    cout << endl;
}
BufferManager::BufferManager():myLRU(){
    dirty.clear();
    for (int i = 0; i < BLOCK_NUM;i++){
        memset(Page[i].pool, 0, BLOCK_SIZE);
        Page[i].setpin(0);
    }
}
int BufferManager::findBlock(std::string content, std::string fileName,int block_id){
    string path = content + fileName + to_string(block_id);
    int find = myLRU.insert(path);
    Page[find].setFile(content, fileName,block_id);
    return find;
} 
int BufferManager::inputBlock(std::string content, std::string fileName, int block_id){
    string path = content + fileName + to_string(block_id);
    int i = myLRU.insert(path);//找到path 对应的block_id
    if(!Page[i].inputFile(content,fileName,block_id)){
        // cout << "BufferManager::file not exists" << endl;
        return -1;
    }
    // myLRU.test();
    return i;
}
int BufferManager::flush(){
    // cout << "diry" << endl;
    for (int i = 0; i < dirty.size(); i++){
        // Page[dirty[i]].prtpool();
        Page[dirty[i]].outputFile();
        // cout<<dirty[i];
    }
    // cout << endl;
    dirty.clear();
    return 1;
}
void BufferManager::clear(std::string content, std::string fileName, int block_id){
    string path = content + fileName + to_string(block_id);
    int id = myLRU.exist(path);
    if(id!=-1){
        memset(Page[id].pool, 0, BLOCK_SIZE);
        Page[id].setpin(0);
        Page[id].setFile(" ", " ", 0);
    }
}