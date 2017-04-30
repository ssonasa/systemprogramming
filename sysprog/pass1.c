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
void pass2(char* filename, char* output);
symrow* symTab[1024];

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

int main(){
    pass1("FIGURE2-5(TAB).txt", "intermediate.txt");
    pass2("intermediate.txt", "output.txt");
}

int hasLabel(char* str){ return !isspace(str[0]); }
int isCommentLine(char* str){ return str[0] == '.'; }
int isExistLabel(char* label){
    if(label==NULL) return 0;

    if(label[0] == '#'){
        // printf("direct");
    }else if(label[0] == '@'){
        // printf("indirect", label[0]);
    }
	for(int i=0; symTab[i] != NULL; i++)
		if(!strcmp(label, symTab[i]->label)) return 1;
	return 0;
}

char* getMachineCode(char* opcode){
	for(int i=0; i<sizeof(machinecodes)/sizeof(char*); i++){
		if(!strcmp(opcodes[i], opcode)){
			return machinecodes[i];
		}else{
        }
	}
	return NULL;
}


int getOpcodeSize(char* opcode){
	for(int i=0; i<sizeof(machinecodes)/sizeof(char*); i++){
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
	symrow* row = (symrow*)malloc(sizeof(symrow)); 
	row->label = label;
	row->loc = loc;
	symTab[i] = row;
	return 0;
}

int getLocFromSYMTAB(char* operand){
    if(operand==NULL) return 0;
	for(int i=0; symTab[i] != NULL; i++)
		if(!strcmp(operand, symTab[i]->label)) return symTab[i]->loc;

    //can't ref address, then set oprand_address to 0.

    // TODO: setError flag;
    // printf("%s, can't refernece operand!\n", operand);
	return 0;
}

void pass1(char* filename, char* output){
    int loc=0, start_address, temp;
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
            sscanf(operand,"%x",&loc); // start address.
            start_address = loc; // set start_address to locctr
            fprintf(fpw, "%04x|%s\t%s\t%s\n", loc, label, opcode, operand); // write intermediate file.
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
        fprintf(fpw, "%04x|%s", loc, line); // write intermediate file.

		char* code = getMachineCode(opcode);
		if(code != NULL){
            loc += getOpcodeSize(opcode);
            sscanf(code, "%d", &temp);
		}else if(!strcmp(opcode, "WORD")){
            loc += 3;
		}else if(!strcmp(opcode, "RESB")){
            sscanf(operand, "%d", &temp);
            loc += temp;
		}else if(!strcmp(opcode, "RESW")){
            sscanf(operand, "%d", &temp);
            loc += 3*temp;
		}else if(!strcmp(opcode, "BYTE")){
            loc += getBytesLength(operand);
		}else if(!strcmp(opcode, "BASE")){
            
        }else{
			printf("\nError: Invalid operation code: %s", opcode); break;

		}
	}

    fprintf(fpw, ".length: %04x", loc - start_address); // write intermediate file.

	//END Pass 1
    fclose(fp);
    fclose(fpw);
}

int parseLoc(char* line){
    int ref;
    char* loc = (char*)malloc(strlen(line));
    strcpy(loc, line);

    strtok(loc, "|");
    strcpy(line, strtok(NULL, ""));
    
    sscanf(loc,"%x",&ref); // start address.
    return ref;
}

void pass2(char* filename, char* output){
	char line[1024];
	FILE* fp = fopen(filename, "r");
    FILE* fpw = fopen(output, "wt");
    int text_record_cnt=9;

	for(int linenum=1; fgets(line, LINE_BUF_SIZE, fp); linenum++){
		if (isCommentLine(line)) {
            //write listing line
            continue;
        }

        int loc = parseLoc(line);
        char** cols = parseLine(line);
		char* label = cols[0];
		char* opcode = cols[1];
		char* operand = cols[2];
        int operand_address = 0;

        if(linenum == 1 && !strcmp(opcode, "START")) { 
            //TODO: write listingline
            //TODO: write header record to object program
            fprintf(fpw, "H");
            fprintf(fpw, "%s", label);
            

            //TODO: initialize first text record
            continue;
        }

        if(!strcmp(opcode, "END")) break;
        
        char* code = getMachineCode(opcode);
        if(code != NULL){
            // printf("%d", getOpcodeSize(opcode));
            if(isExistLabel(operand)){
                operand_address = getLocFromSYMTAB(operand);
            }else{
                operand_address = 0;
            }
            
            //TODO: assamble the object code instruction

        }else if(!strcmp(opcode, "WORD")){
            //TODO: convert to constant object code
		}else if(!strcmp(opcode, "BYTE")){
            //TODO: convert to constant object code
        }

        if(text_record_cnt<9){ //obejct code will not fit into the current Text record
            text_record_cnt++;   
        }else{
            fprintf(fpw, "\nT");
            fprintf(fpw, "%06x", loc);
            printf("\n");
            printf("%06x/", loc);
            text_record_cnt = 0;
        }
            printf("%s%04x/", code, operand_address);
            fprintf(fpw, "%s%04x/", code, operand_address);
    }

    //write Text record to object program
    //write end text to object program

    fclose(fp);
    fclose(fpw);
}