#include"head.h"
using namespace std;
std::string getString(char a[], int len){
    string x;
    for (int i = 0; i < len; i++){
        x.push_back(a[i]);
    }
    return x;
}
void strtochar(char a[], std::string str){
    for (int i = 0; i < str.size(); i++){
        a[i] = str[i];
    }
}
int char4int(char a[]){
    int x;
    memcpy(&x, a, 4);
    return x;
}
void intchar4(char a[],int x){
    memcpy(a, &x, 4);
}
float char4float(char a[]){
    float b;
    memcpy(&b, a, 4);
    return b;
}
void floatchar4(char a[], float x){
    memcpy(a, &x, 4);
}
bool mycondition::satify(std::string input,int i){
    string temp1 = input;
    string temp2 = this->value[i];
    switch (this->op[i]){
    case 0:
        if (temp1 != temp2)
            return 0;
        break;
    case 1:
        if (temp1 == temp2)
            return 0;
        break;
    case 2:
        if (temp1 >= temp2)
            return 0;
        break;
    case 3:
        if (temp1 <= temp2)
            return 0;
        break;
    case 4:
        if (temp1 > temp2)
            return 0;
        break;
    case 5:
        if (temp1 < temp2)
            return 0;
        break;
    }
    return 1;
} 
bool mycondition::satify(int input,int i){
    int temp1 = input;
    int temp2 = stoi(this->value[i]);
    switch (this->op[i]){
    case 0:
        if (temp1 != temp2)
            return 0;
        break;
    case 1:
        if (temp1 == temp2)
            return 0;
        break;
    case 2:
        if (temp1 >= temp2)
            return 0;
        break;
    case 3:
        if (temp1 <= temp2)
            return 0;
        break;
    case 4:
        if (temp1 > temp2)
            return 0;
        break;
    case 5:
        if (temp1 < temp2)
            return 0;
        break;
    }
    return 1;
}
bool mycondition::satify(float input, int i){
    float temp1 = input;
    float temp2 = stof(this->value[i]);
    switch (this->op[i]){
    case 0:
        if (temp1 != temp2)
            return 0;
        break;
    case 1:
        if (temp1 == temp2)
            return 0;
        break;
    case 2:
        if (temp1 >= temp2)
            return 0;
        break;
    case 3:
        if (temp1 <= temp2)
            return 0;
        break;
    case 4:
        if (temp1 > temp2)
            return 0;
        break;
    case 5:
        if (temp1 < temp2)
            return 0;
        break;
    }
    return 1;
}
mycondition mycondition::project(std::string colName){
    mycondition pro;
    for (int i = 0; i < this->type.size(); i++){
        if(colName==this->col[i]){
            pro.op.push_back(this->op[i]);
            pro.value.push_back(this->value[i]);
            pro.type.push_back(this->type[i]);
        }
    }
    return pro;
}
// mycondition 