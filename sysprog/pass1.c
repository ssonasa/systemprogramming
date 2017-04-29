#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define LINE_BUF_SIZE 1024
#define MAX_LINES 1024


typedef struct symrow{
	char* label;
	int loc;
}symrow;

void pass1(char* filename, char* output);
symrow* symTab[1024];

char* opcodes[] = {"ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP", "COMPF", "COMPR", "DIV", "DIVF",
                        "DIVR", "FIX", "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT", "JSUB", "LDA",
                        "LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
                        "MULR", "NORM", "OR", "RD", "RMO", "RSUB", "SHIFTL", "SHIFTR", "SIO", "SSK",
                        "STA", "STB", "STCH", "STF", "STI", "STL", "STS", "STSW", "STT", "STX",
                        "SUB", "SUBF", "SUBR", "SVC", "TD", "TIO", "TIX", "TIXR", "WD", "+JSUB"};

char* machinecodes[] = {"18", "58", "90", "40", "B4", "28", "88", "A0", "24", "64",
                            "9C", "C4", "C0", "F4", "3C", "30", "34", "38", "48", "00",
                            "68", "50", "70", "08", "6C", "74", "04", "D0", "20", "60",
                            "98", "C8", "44", "D8", "AC", "4C", "A4", "A8", "F0", "EC",
                            "0C", "78", "54", "80", "D4", "14", "7C", "E8", "84", "10",
                            "1C", "5C", "94", "B0", "E0", "F8", "2C", "B8", "DC", "48"};

int size[] = {3, 3, 2, 3, 2, 3, 3, 2, 3, 3,
                2, 1, 1, 1, 3, 3, 3, 3, 3, 3, 
                3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
                2, 1, 3, 3, 2, 3, 2, 2, 1, 3,
                3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
                3, 3, 2, 2, 3, 1, 3, 2, 3, 4};

int main(){
    // printf("%s\n", machinecodes[60]);
    pass1("FIGURE2-5(TAB).txt", "FIGURE2-5(TAB)_OUT.txt");
}

int hasLabel(char* str){ return !isspace(str[0]); }
int isCommentLine(char* str){ return str[0] == '.'; }
int isExistLabel(char* label){
	for(int i=0; symTab[i] != NULL; i++)
		if(!strcmp(label, symTab[i]->label)) return 1;
	return 0;
}

char* getMachineCode(char* opcode){
    // printf("[%s]", opcode);
	// for(int i=0; opcodes[i]!=NULL; i++){
	for(int i=0; i<60; i++){
		if(!strcmp(opcodes[i], opcode)){
            printf("[%d, opcodes: %s]", i, opcode);
			return machinecodes[i];
		}else{
        }
	}
	return NULL;
}

int getOpcodeSize(char* opcode){
	for(int i=0; opcodes[i]!=NULL; i++){
		if(!strcmp(opcodes[i], opcode)){
			return size[i];
		}
	}
	return 0;
}

int getBytesLength(char* operand){
    if(operand[0] == 'C'){
        return strlen(operand)-3;
    }else{
        return 1;
    }
}


char** parseLine(char* line){
    int i=0;
    char* col[10] = {};
    char* ch = (char*)malloc(strlen(line));
    strcpy(ch, line);

    if (!hasLabel(line)) col[i++] = "\t";
    ch = strtok(ch, " 	\n");
    for(; ch != NULL; i++){
        col[i] = (char*) malloc(strlen(ch));
        strcpy(col[i], ch);
        ch = strtok(NULL, " 	\n");
    }
    
    return col;
}

int addSYMTAB(char* label, int loc){
	int i;
	for(i=0; symTab[i] != NULL; i++){}
	// printf("\nADD SYM[%d]: %s", i, label);
	symrow* row = (symrow*)malloc(sizeof(symrow)); 
	row->label = label;
	row->loc = loc;
	symTab[i] = row;
	return 0;
}

void pass1(char* filename, char* output){
    int loc=0, start_address, temp;;
	char line[1024];
	FILE* fp = fopen(filename, "r");
    FILE* fpw = fopen(output, "wt");

	for(int linenum=1; fgets(line, LINE_BUF_SIZE, fp); linenum++){
		if (isCommentLine(line)) continue;

        char** cols = parseLine(line);
		char* label = cols[0];
		char* opcode = cols[1];
		char* operand = cols[2];

        if(!strcmp(opcode, "END")) break;

        if(linenum == 1 && !strcmp(opcode, "START")) { 
            start_address = loc; // set start_address to locctr
            sscanf(operand,"%x",&loc); // start address.
            fprintf(fpw, "%04x\t%s\t%s\t%s\n", loc, label, opcode, operand); // write intermediate file.
            continue;
        }

        
        //[Label Check]
		if(strcmp(label, "\t")){
			if(isExistLabel(label)){
				printf("\nError: Duplicated label: %s", label); break;
			}else{
				addSYMTAB(label, loc);
			}
		}

		//search optab for opcode
		char* code = getMachineCode(opcode);
		if(code != NULL){ //TODO: add 3 {instruction length} to LOCCTR
            loc += getOpcodeSize(opcode);
            // sscanf(code, "%d", &temp);
            // loc += temp;
		}else if(!strcmp(opcode, "WORD")){ // TODO: add 3 to LOCCTR
            loc += 3;
		}else if(!strcmp(opcode, "RESB")){  // TODO: add #[OPERAND] LOCCTR
            sscanf(operand, "%d", &temp);
            loc += temp;
		}else if(!strcmp(opcode, "RESW")){  // TODO: add 3*#[OPERAND] to LOCCTR
            sscanf(operand, "%d", &temp);
            loc += 3*temp;
		}else if(!strcmp(opcode, "BYTE")){

            // loc += getBytesLength(operand);
			//TODO
			// 1. find length of constant in bytes.
			// 2. add length to LOCCTR.
		}else{
			// printf("%s", opcode);
			// printf("\nError: Invalid operation code: %s", opcode); break;
			//TODO: invalid opcode error.
		}


		//TODO
        printf("%s", line); // write intermediate file.
        fprintf(fpw, "%04x \t %s", loc, line); // write intermediate file.
		// write line to intermediate file.

	}



	//TODO
	// write last line to intermediate file.
	// save (LOCCTR - starting address) as program length


	//END Pass 1
    fclose(fp);
    fclose(fpw);
}