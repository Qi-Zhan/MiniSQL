#ifndef _INTERPRETER_H
#define _INTERPRETER_H
#include"head.h"
#include<regex>
#include<time.h>
std::vector<std::string> split(std::string str, std::string pattern);
void strCleaner(std::string &str);
int onestep(std::string str, std::vector<std::regex> pattern, instruction &Instruction);
void loadPattern(std::vector<std::regex> &pattern);
int execfile(std::string filestr);


#endif