#ifndef _API_H
#define _API_H
#include"head.h"
#include"CatalogManager.h"
#include"RecordManager.h"
#include"IndexManager.h"
class API{
public:
    void mainAPI();//调用其他模块运行指令
    void getins(instruction ins);//二级译码
private:
    mycondition condition;
    instruction ins;
    std::string primary;
    bool checkprimary();
    
};
#endif