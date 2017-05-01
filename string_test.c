#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#define LINE_BUF_SIZE 1024
#define MAX_LINES 1024



void ssum(char** dest, char* str1, char* str2){
    char* buf;

    buf = (char*)malloc(strlen(str1)*sizeof(char)+strlen(str2)*sizeof(char));
    strcat(buf, str1);
    strcat(buf, str2);
    *dest = buf;
}

void sappend(char** str1, char* str2){
    char* buf;

    printf("--%d--", strlen(*str1));
    buf = (char*)malloc(strlen(*str1)*sizeof(char)+strlen(str2)*sizeof(char));
    strcat(buf, *str1);
    strcat(buf, str2);
    *str1 = buf;
}

void sinsert(char** str1, int idx, char *str2){
    char* buf;
    char* sub;
    sub = (char*)malloc(idx);
    buf = (char*)malloc(strlen(*str1)*sizeof(char)+strlen(str2)*sizeof(char));
    memcpy(sub, *str1, idx);
    strcat(buf, sub);
    strcat(buf, str2);
    sub = (char*)malloc(strlen(*str1)-idx);
    memcpy(sub, *str1+idx, strlen(*str1)-idx);
    strcat(buf, sub);

    *str1 = buf;
}

int main(){

    char* string1 = "Hello";
    char* string2 = "World";
    char* result = "sdf";

    char* strs[10] = {"Hello1234", "World", "test"};
    
    printf("%s(%d)\n", strs[0], strs[0]);
    printf("size:%d, %d, %d\n", strlen(strs[0])*sizeof(char), strlen(strs[1])*sizeof(char), strlen(result)*sizeof(char));
    // sappend(&strs[0], strs[1]);
    // strs[1] = result;
    printf("%s(%d)\n", strs[0], strs[0]);
    printf("size:%d, %d, %d\n", strlen(strs[0])*sizeof(char), strlen(strs[1])*sizeof(char), strlen(result)*sizeof(char));



    sinsert(&strs[0], 9, "test");

    for(int i=0; i<3;i++){
        printf("\n%s", strs[i], strs[i]);
    }

    return 0;
}