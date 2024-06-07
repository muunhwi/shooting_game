#include "Common.h"
char* getFile(const char* fileName)
{
    std::ifstream ifs;
    ifs.open(fileName, std::ios::in | std::ios::ate);

    if (!ifs.is_open())
    {
        std::cerr << "Failed to open the " << fileName << '\n';
        return nullptr;
    }

    std::streamsize size = ifs.tellg();
    ifs.seekg(0);

    char* buffer = (char*)malloc((size_t)size);
   
    ifs.read(buffer, size);
    std::streamsize readSize = ifs.gcount();

    if(buffer)
        buffer[readSize] = '\0';

    ifs.close();


    return buffer;
}


bool my_atoi(int* ret, char* str)
{
    int k = 0;
    int sign = 1;

    if (!str)
        return false;

    if (str[0] == '-')
    {
        sign = -1;
        str++;
    }

    while (*str) {

        if ((*str) < '0' || (*str) > '9')
            return false;
       
        k = (k * 10) + (*str) - '0';
        str++;
    }

    *ret = k * sign;
    return true;
}

int getTokenList(char* data, const char* del, char** tokenList)
{
    char* context = nullptr;
    char* token;
    int tokenNum = 0;

    token = strtok_s(data, del, &context);
    while (token)
    {
        tokenList[tokenNum++] = token;
        token = strtok_s(nullptr, del, &context);
    }

    return tokenNum;
}

