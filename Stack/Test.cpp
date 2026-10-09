#include <stdio.h>

#define CALL_INFO(str, comment) makeInfoStr(str, __FILE_NAME__, __LINE__, __func__, comment)

void makeInfoStr(char* const str, const char* const file_name, unsigned int line, const char* const func_name, const char* const comment){

    sprintf(str, "Check of stack was called in %s:%d in function %s, %s", file_name, line, func_name, comment);
}

int main(){

    char buffer[100] = {};

    CALL_INFO(buffer, "This is my comment yoo");

    printf("%s", buffer);

}

