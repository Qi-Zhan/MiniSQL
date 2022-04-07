#ifndef _HEAD_H
#define _HEAD_H
#include <stdio.h>
#include <iostream>
#include <string>
#include <iterator>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>
#include<cmath>
class instruction{
public:
    int number;                    //指令数
    std::vector<std::string> info; //pattern 识别出的参数
};
class mycondition{
public:
    std::vector<std::string> col;
    //实现对某一col copy赋值
    // mycondition();
    mycondition project(std::string colName);//投影到某个col 为index设计
    bool satify(std::string input,int i);
    bool satify(int input,int i);
    bool satify(float input,int i);
    std::vector<std::string> value;
    std::vector<int> op;  //0=	1<>	2<	3>	4<=	5>=
    std::vector<int> type;//0 int 1 string
};

class record{
public:
    int block_id;
    int offset;
    record(int a, int b) { block_id = a;
        offset = b;
    };
};
std::string getString(char a[], int len);
void strtochar(char a[], std::string str);
int char4int(char a[]);
float char4float(char a[]);
void intchar4(char a[], int x);
void floatchar4(char a[], float x);
#endif
