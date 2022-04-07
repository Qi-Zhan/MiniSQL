#include"BPlusTree.h"
#define mid(N) (N / 2) //考虑define 常数简化设计
using namespace std;
int BPlusTree::insertKey(std::string key, record pointer){
    uselist.clear();
    int base;
    int offset;
    int root = buffer.inputBlock("index", indexName, 1);
    if (char4int(buffer.Page[root].pool + 1)){ //根非空
        //可以都集成一个函数 如insertEmptyRoot
        if (isLeaf(root)){ //root is leaves
            insertLeaf(root, key, pointer);
            if (leafFull(root)){
                splitRoot(root); // split
            }
        }else{ //root is internal
            int last = root;
            while (isInternal(last)){
                uselist.push_back(last);
                base = 5;
                offset = 4 + size;
                int keyNum = keyNumber(last);
                int i;
                string a;
                for (i = 0; i < keyNum; i++)
                {
                    a = getString(buffer.Page[last].pool + base + offset * i + 4, size);
                    if (compare(key, a) >= 0)
                        last = char4int(buffer.Page[last].pool + base + offset * i);
                }
            }
                base = 17;
                offset = 8 + size;
                int keyNum = keyNumber(root);
                int i;
                string a;
                for (i = 0; i < keyNum; i++)
                {
                    a = getString(buffer.Page[last].pool + base + offset * i + 8, size);
                    if (compare(key, a) == 0)
                    {
                        int x = char4int(buffer.Page[last].pool + base + offset * i);
                        int y = char4int(buffer.Page[last].pool + base + offset * i + 4);
                        record onerecord(x, y);
                    } 
                }
        }
    }
    else{ //根空
        intchar4(buffer.Page[root].pool + 1, 1);
        insertoneLeaf(root, key, pointer, 9);
    }
    // std::cout << "BPlusTreeInsert" << key << std::endl;
    // test(root);
    buffer.flush();
    return 1;
}
int BPlusTree::compare(std::string a, std::string b){
    if(type==-1){
        float a1 = stof(a);
        float a2 = stof(b);
        if(a1==a2)
            return 0;
        return (a1 > a2) ? 1 : -1;
    }else if(type==0){
        int a1 = stoi(a);
        int a2 = stoi(b);
        if (a1 == a2)
            return 0;
        return (a1 > a2) ? 1 : -1;
    }else{
        if (a == b)
            return 0;
        return a > b ? 1 : -1;
    }
}
void BPlusTree::copyto(std::string key, char a[]){
    if (type == 0){
        int a1 = stoi(key);
        intchar4(a, a1);
    }else if(type==-1){
        float a1 = stoi(key);
        floatchar4(a, a1);
    }else
        strtochar(a, key);
}
void BPlusTree::insertoneLeaf(int block_id, std::string key, record onerecord, int base){
    intchar4(buffer.Page[block_id].pool + base, onerecord.block_id);
    intchar4(buffer.Page[block_id].pool + base + 4, onerecord.offset);
    if (type == 0){
        intchar4(buffer.Page[block_id].pool + base+8, stoi(key));
    }
    else if (type == -1){
        floatchar4(buffer.Page[block_id].pool + base+8, stof(key));
    }
    else
        strtochar(buffer.Page[block_id].pool + base+8, key);
}
int BPlusTree::getFreeBlock(){
    int head = buffer.inputBlock("record", indexName, 0);
    int len = buffer.Page[head].pool[0];
    int len2 = buffer.Page[head].pool[1 + len];
    int Free = char4int(buffer.Page[head].pool + len + 2 + len2);//
    intchar4(buffer.Page[head].pool + len + len2 + 2, Free + 1);//
    return Free;
    // return getString(buffer.Page[head].pool + len + 2, len2);
}
void BPlusTree::moveKey(int dst, int src, int begin, int end, int start,int offset,int baseDst,int baseSrc){
    char a[BLOCK_SIZE];//dst src可能是同一个所以要copy一份
    memcpy(a, buffer.Page[src].pool, BLOCK_SIZE);
    for (int i = begin; i <= end; i++){
        memcpy(buffer.Page[dst].pool+baseDst+offset*(i-begin+start), a+baseSrc+i*offset, offset);//
    }
}
void BPlusTree::test(int index_id){
    int size = (type <= 0) ? 4 : type;
    if(isLeaf(index_id)){
        cout << "it's leaf" << endl;
    }
    if(isInternal(index_id)){
        cout << "it's internal node" << endl;
    }
    int keyNum = char4int(buffer.Page[index_id].pool + 1);
    int point = 5;
    cout << "exist " << keyNum << " keys" << endl;
    cout << "parent id = " << char4int(buffer.Page[index_id].pool + point)<<endl;
    point += 4;
    if (isLeaf(index_id)){
        for (int i = 0; i < keyNum; i++){
            cout << "child id = " << char4int(buffer.Page[index_id].pool + point) << endl;
            point += 4;
            cout << "child key = " << char4int(buffer.Page[index_id].pool + point) << endl;
            point += 4;
        }
    }
    if (isInternal(index_id)){
        for (int i = 0; i < keyNum; i++){
            cout << "block id = " << char4int(buffer.Page[index_id].pool + point) << endl;
            point += 4;
            cout << "block offset = " << char4int(buffer.Page[index_id].pool + point) << endl;
            point += 4;
            cout << "key = "<<getString(buffer.Page[index_id].pool + point, size);
            point += size;
        }
    }
    return;
}
record BPlusTree::findKey(std::string key, int root){
    int head = buffer.inputBlock("index", indexName, root);
    uselist.push_back(head);
    int base, offset;
    if(isInternal(root)){
        base = 5;
        offset = 4 + size;
        int keyNum = keyNumber(head);
        int i;
        string a;
        for (i = 0; i < keyNum; i++){
            a = getString(buffer.Page[head].pool + base + offset * i + 4, size);
            if (compare(key, a) >= 0){
                findKey(key, char4int(buffer.Page[head].pool + base + offset * i));
            } 
        }
    }else if(isLeaf(root)){
        base = 17;
        offset = 8 + size;
        int keyNum = keyNumber(root);
        int i;
        string a;
        for (i = 0; i < keyNum; i++){
            a = getString(buffer.Page[head].pool + base + offset * i + 8, size);
            if (compare(key, a)==0){
                int x = char4int(buffer.Page[head].pool + base + offset * i);
                int y = char4int(buffer.Page[head].pool + base + offset * i+4);
                record onerecord(x, y);
            } //第一个key大的,即要插入的地方
        }
    }else{
        cout << "Leaf Internal error" << endl;
    }
    record onerecord(0, 0);
    return onerecord;
}
int BPlusTree::split(int block_id){
    buffer.Page[block_id].pool[0] = 'I';
    buffer.setdirty(block_id);
    int a = getFreeBlock();
    buffer.setdirty(a);
    moveKey(a, block_id, 0, floor(Branch / 2), 0, 8 + size, 9, 9);
    return 1;
}
int BPlusTree::insertkey(std::string key, record pointer){
    return 1; //just for test
}
int BPlusTree::splitRoot(int block_id){
    buffer.Page[block_id].pool[0] = 'I';
    buffer.setdirty(block_id);
    int a = getFreeBlock();
    int b = getFreeBlock();
    buffer.setdirty(a);
    buffer.setdirty(b);
    moveKey(a, block_id, 0, floor(Branch / 2), 0, 8 + size, 9, 9);
    return 1;
}
int BPlusTree::insertLeaf(int block_id, std::string key, record onerecord){
    int keyNum = keyNumber(block_id);
    int base = 9;
    int offset = 4 + 4 + size;
    int i;
    for (i = 0; i < keyNum; i++){
        string a = getString(buffer.Page[block_id].pool + base + offset * i + 8,size);
        if(compare(key,a)>=0)//第一个key大的,即要插入的地方
            break;
    }
    moveKey(block_id, block_id, i, keyNum - 1, i + 1, offset, base,base);
    insertoneLeaf(block_id, key, onerecord, base + i * offset);
    return 1;
}
int BPlusTree::deleteKey(mycondition condition,std::vector<record> &recordList,int root){
    int head = buffer.inputBlock("index", indexName, root);
    uselist.push_back(head);
    int base, offset;
    if (isInternal(root))
    {
        base = 5;
        offset = 4 + size;
        int keyNum = keyNumber(head);
        int i;
        string a;
        for (i = 0; i < keyNum; i++)
        {
            int flag = 1;
            a = getString(buffer.Page[head].pool + base + offset * i + 4, size);
            for (int j = 0; j < condition.type.size(); j++){
                if(condition.satify(a,j)==0){
                    flag = 1;
                    break;
                }
            }
            if (flag==1){
                deleteKey(condition,recordList,char4int(buffer.Page[head].pool + base + offset * i));
            }
        }
    }
    else if (isLeaf(root))
    {
        base = 17;
        offset = 8 + size;
        int keyNum = keyNumber(root);
        int i;
        string a;
        for (i = 0; i < keyNum; i++)
        {
            int flag = 1;
            a = getString(buffer.Page[head].pool + base + offset * i + 8, size);
            for (int j = 0; j < condition.type.size(); j++)
            {
                if (condition.satify(a, j) == 0)
                {
                    flag = 0;
                    break;
                }
            }
            if (flag == 1){
                int x = char4int(buffer.Page[head].pool + base + offset * i);
                int y = char4int(buffer.Page[head].pool + base + offset * i + 4);
                record onerecord(x, y);
                recordList.push_back(onerecord);
                moveKey(head, head, i + 1, keyNum, i, offset, base, base);

            }
            
        }

    }
    else
    {
        cout << "Leaf Internal error" << endl;
    }
    return 1;
}
