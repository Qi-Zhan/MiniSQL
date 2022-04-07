#include<iostream>
#include<map>
#include<list>
#include<string>
#include<iterator>
using namespace std;
#define BLOCK_NUM 8
class LRU{
public:
    LRU();
    int insert(string path);
    void test();
private:
    map<string, list<int>::iterator> myhash;
    list<int> mylist;
    int free;
};
LRU::LRU(){
    free = 0;
    mylist.clear();
    myhash.clear();
}
int LRU::insert(string path){
    return 
    if(myhash.find(path)!=myhash.end()){//该块已经在buffer中了
        list<int>::iterator iter = myhash.find(path)->second;
        mylist.erase(iter);
        mylist.push_front(*iter);
        return *mylist.begin();
    }else if(free<BLOCK_NUM){//有空余的位置
        mylist.push_front(free++);
        myhash[path] = mylist.begin();
        return free - 1;
    }else{//替换最后一个
        map<string, list<int>::iterator>::iterator iter = myhash.begin();
        int x = *prev(mylist.end());
        for (; iter != myhash.end();){
            if(*iter->second==x)
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
    list<int>::iterator iter = mylist.begin();
    for (; iter!=mylist.end(); iter++){
        cout << *iter << " ";
    }
    cout << endl;
}
int main(){
    LRU mylru;
    for (int i = 0; i < 20;i++){
        string a;
        cin >> a;
        cout<<mylru.insert(a)<<endl;
        mylru.test();
    }
}