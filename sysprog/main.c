#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define LINE_BUF_SIZE 1024
#define MAX_LINES 1024
#define opcodeNum 60

typedef struct symrow{
	char* label;
	int loc;
}symrow;


char* opcodes[] = {"ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP", "COMPF", "COMPR", "DIV", "DIVF",
                        "DIVR", "FIX", "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT", "JSUB", "LDA",
                        "LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
                        "MULR", "NORM", "OR", "RD", "RMO", "RSUB", "SHIFTL", "SHIFTR", "SIO", "SSK",
                        "STA", "STB", "STCH", "STF", "STI", "STL", "STS", "STSW", "STT", "STX",
                        "SUB", "SUBF", "SUBR", "SVC", "TD", "TIO", "TIX", "TIXR", "WD"};

char* machinecodes[] = {"18", "58", "90", "40", "B4", "28", "88", "A0", "24", "64", 
                            "9C", "C4", "C0", "F4", "3C", "30", "34", "38", "48", "00",
                            "68", "50", "70", "08", "6C", "74", "04", "D0", "20", "60",
                            "98", "C8", "44", "D8", "AC", "4C", "A4", "A8", "F0", "EC",
                            "0C", "78", "54", "80", "D4", "14", "7C", "E8", "84", "10",
                            "1C", "5C", "94", "B0", "E0", "F8", "2C", "B8", "DC"};



// Predefine functions, variables.
void file_open();
void printMap();
void processPass1();
symrow* symTab[1024];
char* asmMap[1024][10] = {0};


int main() {

	file_open("FIGURE2-5(TAB).txt");
	processPass1();
	printMap();
    return 0;
}

int hasLabel(char* str){
	return !isspace(str[0]);
}

int isCommentLine(char* str){
	return str[0] == '.';
}

int determineLOCCTR(){
	if(strcmp(asmMap[0][1], "START")){
		//TODO: save #[OPERAND] as starting address
		//TODO: init LOCCTR to starting address
		//TODO: write line to intermediate file
		return 1000; 
	}else{
		return 0;
	}
}

int addSYMTAB(char* label, int loc){
	int i;
	for(i=0; symTab[i] != NULL; i++){}
	printf("\nADD SYM[%d]: %s", i, label);
	symrow* row = (symrow*)malloc(sizeof(symrow)); 
	row->label = label;
	row->loc = loc;
	symTab[i] = row;
	return 0;
}

int isExistLabel(char* label){
	for(int i=0; symTab[i] != NULL; i++)
		if(!strcmp(label, symTab[i]->label)) return 1;
	return 0;
}

char* getMachineCode(char* opcode){
	for(int i=0; opcodes[i]!=NULL; i++){
		if(!strcmp(opcodes[i], opcode)){
			return machinecodes[i];
		}
	}
	return NULL;
}

void processPass1(){
	int loc = determineLOCCTR();

	for(int i=0;asmMap[i][0] != NULL; i++){
		char* label = asmMap[i][0];
		char* opcode = asmMap[i][1];
		char* line = *asmMap[i];

		if(!strcmp(opcode, "START")) {
			determineLOCCTR();
			continue;
		}

		if(!strcmp(opcode, "END")) break;

		//[Label Check]
		if(hasLabel(line)){
			if(isExistLabel(label)){
				printf("\nError: Duplicated label: %s", label); break;
			}else{
				//TODO: calc LOCCRT
				//loc = ~~~~
				addSYMTAB(label, loc);
			}
		}

		//search optab for opcode
		char* code = getMachineCode(opcode);
		if(code != NULL){
			//TODO: add 3 {instruction length} to LOCCTR
		}else if(!strcmp(opcode, "WORD")){
			//TODO: add 3 to LOCCTR
		}else if(!strcmp(opcode, "RESB")){  // RESB == Byte variable
			//TODO: add [OPERAND] LOCCTR
		}else if(!strcmp(opcode, "RESW")){  // RESW == Word variable (Some Bytes) 
			//TODO: add 3*[OPERAND] to LOCCTR
		}else if(!strcmp(opcode, "BYTE")){
			//TODO
			// 1. find length of constant in bytes.
			// 2. add length to LOCCTR.
		}else{
			// printf("%s", opcode);
			printf("\nError: Invalid operation code: %s", opcode); break;
			//TODO: invalid opcode error.
		}


		//TODO
		// write line to intermediate file.
	}

	//TODO
	// write last line to intermediate file.
	// save (LOCCTR - starting address) as program length


	//END Pass 1
}

void file_open(char* filename){
	char line[1024];
	int linenum = 0;
	FILE* fp = fopen(filename, "r");
	FILE* fpw = fopen("C:\\Users\\손\\Source\\Repos\\systemprogramming\\pass1\\pass1\\intermediate.txt", "wt");


	while(fgets(line, LINE_BUF_SIZE, fp)){
		int i=0;
		if (!hasLabel(line)) asmMap[linenum][i++] = "\t";
		if (isCommentLine(line)) continue;

		char* ch = strtok(line, " 	\n");
		for(; ch != NULL; i++){
			asmMap[linenum][i] = (char*) malloc(strlen(ch));
			strcpy(asmMap[linenum][i], ch);
			ch = strtok(NULL, " 	\n");
			//matchingMnemonicToOpcode (asmMap의 Mnemonic부분과 Opcode를 일치시켰을때의 machinecode)
			if (i == 1) {
				for (int j = 0; j < opcodeNum - 1; j++) {
					if (strcmp(opcodes[j], asmMap[linenum][1]) == 0) {
						//printf("%s = \t", asmMap[linenum][1]);
						asmMap[linenum][1] = machinecodes[j];
						printf("%s\n", asmMap[linenum][1]);
						//printf("%s\n", asmMap[linenum]);
					}//찾았을때 더이상 찾지 않고 바로 linenum++해야 되는데...
				}
			}//여기까지 matchingMnemonicToOpcode.

			//makes intermediate file
			if (!(strcmp(asmMap[linenum][0], "\t")>0)) {
				fprintf(fpw, "\t");
				fprintf(fpw, asmMap[linenum][i]);
			}
			else{
				fprintf(fpw, asmMap[linenum][i]);
				fprintf(fpw, "\t");
			}//makes intermediate file

		}
		linenum++;
		fprintf(fpw, "\n");
	}

	fclose(fp);
	fclose(fpw);
}


void printMap(){
	for(int i=0;asmMap[i][0] != NULL; i++){
		for(int j=0; asmMap[i][j]!=NULL; j++)
			if(asmMap[i][j]!=NULL) printf("%s/", asmMap[i][j]);
		printf("\n");
	}
}

