#include "sic.h"

symrow* symTab[1024];

int main(int argc, char* argv[]){

    // if (argc != 3) {
    //     fputs("Usage: $sic source.asm output.txt\n", stderr);
    //     exit(1);
    // }

    // pass1(argv[1], "intermediate.txt");
    // pass2("intermediate.txt", argv[2]);

    pass1("source.asm", "intermediate.txt");
    pass2("intermediate.txt", "output.txt");
    // printf("Complete!");
}


int hasLabel(char* str){ return !isspace(str[0]); }
int isCommentLine(char* str){ return str[0] == '.'; }
int isExistLabel(char* label){
    if(label==NULL) return 0;
	for(int i=0; symTab[i] != NULL; i++)
		if(!strcmp(label, symTab[i]->label)) return 1;
	return 0;
}
char* getMachineCode(char* opcode){
	for(int i=0; i<sizeof(machinecodes)/sizeof(char*); i++)
		if(!strcmp(opcodes[i], opcode)) return machinecodes[i];

	return NULL;
}
int getOpcodeSize(char* opcode){
	for(int i=0; i<sizeof(machinecodes)/sizeof(char*); i++)
		if(!strcmp(opcodes[i], opcode)) return size[i];

	return 0;
}
int getBytesLength(char* operand){
    if(operand[0] == 'C') 
        return strlen(operand)-3;
    else 
        return 1;
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
	for(i=0; symTab[i] != NULL; i++);
    
	symrow* row = (symrow*)malloc(sizeof(symrow)); 
	row->label = label;
	row->loc = loc;
	symTab[i] = row;
	return 0;
}
int getLocFromSYMTAB(char* operand){
    if(operand==NULL) return 0; //if not exist loc in SYMTAB.
	for(int i=0; symTab[i] != NULL; i++)
		if(!strcmp(operand, symTab[i]->label))
            return symTab[i]->loc;

	return 0;
}
int parseLoc(char* line){
    int ref;
    char* loc = (char*)malloc(strlen(line));
    strcpy(loc, line);

    strtok(loc, "|");
    strcpy(line, strtok(NULL, ""));
    
    sscanf(loc,"%X",&ref);
    return ref;
}



void pass1(char* filename, char* output){
    int loc=0, start_address, temp;
	char line[1024] = {};
	FILE* fp = fopen(filename, "r");
    FILE* fpw = fopen("intermediate.txt", "wt");

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
		if(strcmp(label, "\t"))
			if(isExistLabel(label)){
				printf("\nError: Duplicated label: %s", label); break;
			}else
				addSYMTAB(label, loc);
            
		
        //Write intermediate file.
        fprintf(fpw, "%04x|%s", loc, line); 

        //Search optab for opcode
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

    // Write file length to intermediate file.
    fprintf(fpw, ".L%04X", loc - start_address); 

    fclose(fp);
    fclose(fpw);
}

void pass2(char* filename, char* output){
	char str[1024] = {};
    char buf[1024];
    char* pbuf[1024] = {};
    int line = 0;
    int text_record_cnt = 9;
    int first_instruction = 0;

	FILE* fp = fopen(filename, "r");
    FILE* fpw = fopen(output, "wt");


    for(int linenum=1; fgets(str, LINE_BUF_SIZE, fp); linenum++){
        if (isCommentLine(str)) continue; 

        int loc = parseLoc(str);
        char** cols = parseLine(str);
		char* label = cols[0];
		char* opcode = cols[1];
		char* operand = cols[2];
        if(linenum == 1 && !strcmp(opcode, "START")) {
            sprintf(buf, "H%s  %06X", label, loc);
            sappend(&pbuf[line], buf);
            continue;
        }

        // break for END operand.
        if(!strcmp(opcode, "END")) break;

        if(text_record_cnt<MAX_TEXT_RECORD_SIZE){
            text_record_cnt++; 
        }else{
            line++;
            text_record_cnt=0;
            sprintf(buf, "T%06X", loc);
            sappend(&pbuf[line], buf);
        }

        char* code = getMachineCode(opcode);
        if(code != NULL){
            //Write machineCode + operand_address
            sprintf(buf, "%s%04X", code, getLocFromSYMTAB(operand));
            sappend(&pbuf[line], buf);

        }else if(!strcmp(opcode, "WORD")){
            char* b;
            sscanf(operand, "%d", &b);
            sprintf(buf, "%06X", b);
            sappend(&pbuf[line], buf);

		}else if(!strcmp(opcode, "BYTE")){
             if(operand[0] =='C'){
                char* sub;
                sub = (char*)malloc(strlen(operand)-2);
                memcpy(sub, &operand[2], strlen(operand)-3);  // substring [C'EOF'] => [EOF]
                for(int i=0;i<strlen(sub);i++){
                    sprintf(buf, "%X", sub[i]);
                    sappend(&pbuf[line], buf);
                }
            }else if(operand[0] =='X'){
                char sub[2];
                memcpy(sub, &operand[2], 2);
                sprintf(buf, "%s", sub);
                sappend(&pbuf[line], buf);
            }

        }else if(!strcmp(opcode, "RESW")){
        }else if(!strcmp(opcode, "RESB")){
        }else{
            printf("--------else!-----\n");
        }
    }


    //Insert File length to each line.
    for(int i=1; pbuf[i]!=NULL;i++){
        sprintf(buf, "%02lX", (strlen(pbuf[i])-7)/2);
        sinsert(&pbuf[i], 7, buf);
    }

    //Write end record
    char* sub[6] = {};
    memcpy(sub, &pbuf[0][7], 7);
    sprintf(buf, "E%s", sub);
    sappend(&pbuf[line+1], buf);

    //Write filesize at header record.
    memcpy(sub, &str[2], strlen(str)-1);
    sprintf(buf, "%06s", sub);
    sappend(&pbuf[0], buf);

    //Print console/file all line buf
    for(int i=0; pbuf[i]!=NULL ;i++){
        printf("%s\n", pbuf[i]);
        fprintf(fpw, "%s\n", pbuf[i]);
    }
}