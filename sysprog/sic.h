#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#define LINE_BUF_SIZE 1024
#define MAX_LINES 1024

void pass1(char* filename, char* output);
void pass2(char* filename, char* output);

typedef struct symrow{
	char* label;
	int loc;
}symrow;

char* opcodes[] = {"ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP", "COMPF", "COMPR", "DIV", "DIVF",
                        "DIVR", "FIX", "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT", "JSUB", "LDA",
                        "LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
                        "MULR", "NORM", "OR", "RD", "RMO", "RSUB", "SHIFTL", "SHIFTR", "SIO", "SSK",
                        "STA", "STB", "STCH", "STF", "STI", "STL", "STS", "STSW", "STT", "STX",
                        "SUB", "SUBF", "SUBR", "SVC", "TD", "TIO", "TIX", "TIXR", "WD", "+JSUB", "+LDT"};

char* machinecodes[] = {"18", "58", "90", "40", "B4", "28", "88", "A0", "24", "64",
                            "9C", "C4", "C0", "F4", "3C", "30", "34", "38", "48", "00",
                            "68", "50", "70", "08", "6C", "74", "04", "D0", "20", "60",
                            "98", "C8", "44", "D8", "AC", "4C", "A4", "A8", "F0", "EC",
                            "0C", "78", "54", "80", "D4", "14", "7C", "E8", "84", "10",
                            "1C", "5C", "94", "B0", "E0", "F8", "2C", "B8", "DC", "48", "74"};

int size[] = {3, 3, 2, 3, 2, 3, 3, 2, 3, 3,
                2, 1, 1, 1, 3, 3, 3, 3, 3, 3, 
                3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
                2, 1, 3, 3, 2, 3, 2, 2, 1, 3,
                3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
                3, 3, 2, 2, 3, 1, 3, 2, 3, 4, 4};


void sappend(char** str1, char* str2){
    char* buf;

    if(*str1==NULL) *str1 = "";
    buf = (char*)malloc(strlen(*str1)+strlen(str2)+1);
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