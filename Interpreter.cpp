/*
interpreter实现功能：完成词法分析并合理报错；完成词法分析生成instruction给API
*/
#include "head.h"
#include"Interpreter.h"
#include"API.h"
using namespace std;
API myapi;
vector<string> split(string str, string pattern)
{
    string::size_type pos;
    vector<string> result;
    str += pattern;
    int size = str.size();
    for (int i = 0; i < size; i++)
    {
        pos = str.find(pattern, i);
        if (pos < size)
        {
            string s = str.substr(i, pos - i);
            strCleaner(s);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

void strCleaner(string &str)
{ //去空格去回车
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == '\n')
            str[i] = ' ';
    }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    str.erase(0, str.find_first_not_of("\t"));
    str.erase(str.find_last_not_of("\t") + 1);
}

int onestep(string str, vector<regex> pattern, instruction &Instruction){ //词法分析
    int i;
    smatch res;
    for (i = 0; i < pattern.size(); i++){
        if (regex_match(str, res, pattern[i])){
            break;
        }
    }
    if (i == 0)
        return 0;
    if (i == pattern.size())
        return -1;
    if (i == 10){
        if (execfile(res[1]) == 0)
            return 0;
    }
    if (i != 1 && i != 6 && i != 9 &&i!=7){
        for (int j = 1; j < res.size(); j++)
            Instruction.info.push_back(res[j]);
        return i;
    }
    if (i == 1){ //create table
        Instruction.info.push_back(res[1]);
        vector<string> table = split(res[2], ",");
        for (int j = 0; j < table.size(); j++){
            smatch temp;
            strCleaner(table[j]);
            // cout << table[j] << endl;
            int s;
            for (s = 11; s < 18; s++){
                if (regex_match(table[j],temp, pattern[s])){
                    for (int k = 1; k < temp.size(); k++){
                        Instruction.info.emplace_back(temp[k]);
                    }
                    break;
                }
            }
            if(s==18)
                return -1;
        }
    }
    else if (i == 6 || i == 9)
    { //select where
        Instruction.info.push_back(res[1]);
        vector<string> table = split(res[2], "and");
        for (int j = 0; j < table.size(); j++){
            smatch temp;
            strCleaner(table[j]);
            int s;
            for (s = 18; s < 24; s++){
                if (regex_match(table[j], temp, pattern[s])){
                    for (int k = 1; k < temp.size(); k++){
                        Instruction.info.emplace_back(temp[k]);
                    }
                    break;
                }
            }
            if(s==23)
                return -1;
        }
    }else if(i==7){//insert
        Instruction.info.push_back(res[1]);
        vector<string> table = split(res[2], ",");
        for (int j = 0; j < table.size(); j++){
            Instruction.info.push_back(table[j]);
        }
        
    }
    return i;
}

void loadPattern(vector<regex> &pattern){
    regex temp;
    temp.assign("quit"); //0
    pattern.push_back(temp);
    temp.assign("create[ ]{1,}table[ ]{1,}([\\w]{1,})[ ]{0,}[(](.*)[)]"); //1
    pattern.push_back(temp);
    temp.assign("drop[ ]{1,}table[ ]{1,}([\\w]{1,})"); //2
    pattern.push_back(temp);
    temp.assign("create[ ]{1,}index[ ]{1,}([\\w]{1,})[ ]{1,}on[ ]{1,}([\\w]{1,})[ (]{0,}([\\w]{1,})[ ]{0,}[)]"); //3
    pattern.push_back(temp);
    temp.assign("drop[ ]{1,}index[ ]{1,}([\\w]{1,})"); //4
    pattern.push_back(temp);
    temp.assign("select[ ]{1,}[*][ ]{1,}from[ ]{1,}([\\w]{1,})"); //5
    pattern.push_back(temp);
    temp.assign("select[ ]{1,}[*][ ]{1,}from[ ]{1,}([\\w]{1,})[ ]{1,}where[ ]{1,}(.*)"); //6
    pattern.push_back(temp);
    temp.assign("insert[ ]{1,}into[ ]{1,}([\\w]{1,})[ ]{1,}values[ ]{0,}[(](.*)[)]"); //7
    pattern.push_back(temp);
    temp.assign("delete[ ]{1,}from[ ]{1,}([\\w]{1,})"); //8
    pattern.push_back(temp);
    temp.assign("delete[ ]{1,}from[ ]{1,}([\\w]{1,})[ ]{1,}where[ ]{1,}(.*)"); //9
    pattern.push_back(temp);
    temp.assign("execfile[ ]{1,}(.*)"); //10
    pattern.push_back(temp);
    //create table
    temp.assign("[ ]{0,}(.*)[ ]{1,}(int)[ ]{0,}"); //11
    pattern.push_back(temp);
    temp.assign("[ ]{0,}(.*)[ ]{1,}(float)[ ]{0,}"); //12
    pattern.push_back(temp);
    temp.assign("[ ]{0,}([\\w']{1,})[ ]{1,}(char)[ ]{0,}[(][ ]{0,}([\\d]{1,})[ ]{0,}[)][ ]{0,}"); //13
    pattern.push_back(temp);
    temp.assign("[ ]{0,}(.*)[ ]{1,}(int)[ ]{1,}(unique)[ ]{0,}"); //14
    pattern.push_back(temp);
    temp.assign("[ ]{0,}(.*)[ ]{1,}(float)[ ]{1,}(unique)[ ]{0,}"); //15
    pattern.push_back(temp);
    temp.assign("[ ]{0,}([\\w']{1,})[ ]{1,}(char)[ ]{0,}[(][ ]{0,}([\\d]{1,})[ ]{0,}[)][ ]{1,}(unique)[ ]{0,}"); //16
    pattern.push_back(temp);
    temp.assign("[ ]{0,}(primary key)[ ]{0,}[(](.*)[)][ ]{0,}"); //17
    pattern.push_back(temp);
    //op and
    temp.assign("[ ]{0,}([\\w']{1,})[ ]{0,}(=)[ ]{0,}(.*)"); //18
    pattern.push_back(temp);
    temp.assign("[ ]{0,}([\\w']{1,})[ ]{0,}(>)[ ]{0,}([\\d]{1,})"); //19
    pattern.push_back(temp);
    temp.assign("[ ]{0,}([\\w']{1,})[ ]{0,}(<)[ ]{0,}([\\d]{1,})"); //20
    pattern.push_back(temp);
    temp.assign("[ ]{0,}([\\w']{1,})[ ]{0,}(>=)[ ]{0,}([\\d]{1,})"); //21
    pattern.push_back(temp);
    temp.assign("[ ]{0,}([\\w']{1,})[ ]{0,}(<=)[ ]{0,}([\\d]+)"); //22
    pattern.push_back(temp);
    temp.assign("[ ]{0,}([\\w']{1,})[ ]{0,}(<>)[ ]{0,}([\\d]+)"); //23
    pattern.push_back(temp);
}

int execfile(string filestr){
    // cout << "execfile   " <<filestr<< endl;
    ifstream fin(filestr);
    if (!fin.is_open()){
        cout << "open file failed" << endl;
        return 1;
    }
    vector<regex> pattern;
    loadPattern(pattern);
    clock_t begin, end;
    while (!fin.eof()){
        smatch res;
        string str;
        instruction Instruction;
        getline(fin, str, ';');
        // begin = clock();
        fin.ignore();
        // cout << str << endl;
        strCleaner(str);
        int number = onestep(str, pattern, Instruction);
        if (number == 0)
            return 0;
        else if(number==-1)
            cout << "Interpreter::SyntaxError" << endl;
        else{
            Instruction.number = number;
            myapi.getins(Instruction);
            myapi.mainAPI();
            // end = clock();
            // double elapsed_time = (double)(end - begin) / CLOCKS_PER_SEC;
            // printf("(%3.2f sec)\n", elapsed_time);
        }
    }
    return 1;
}

int main(){
    vector<regex> pattern;
    loadPattern(pattern);
    string str;
    cout << "Minisql>";
    clock_t begin, end;
    getline(cin, str, ';');
    cin.ignore();
    strCleaner(str);
    begin = clock();
    while (1){
        instruction Instruction;
        int i = onestep(str, pattern, Instruction);
        if (i == -1)
            cout << "Interpreter::SyntaxError" << endl;              
        else if (i == 0)//quit
            return 0;
        else{
            Instruction.number = i;
            myapi.getins(Instruction);
            myapi.mainAPI();
            end = clock();
            double elapsed_time = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("(%3.2f sec)\n", elapsed_time);
        }
        cout << "Minisql>";
        getline(cin, str, ';');
        begin = clock();
        cin.ignore();
        strCleaner(str);
    }
    return 0;
}