#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

constexpr int MAX_BUFFER_SIZE = 1024;

char* getFile(const char* FileName);
bool my_atoi(int* ret, char* snum);
int getTokenList(char* data, const char* del, char** tokenList);
