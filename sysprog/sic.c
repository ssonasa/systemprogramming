#include "sic.h"

symrow* symTab[1024];

int main(int argc, char* argv[]){

    // if (argc != 3) {
    //     fputs("Usage: $sic source.asm output.txt\n", stderr);
    //     exit(1);
    // }

    // pass1(argv[1], "intermediate.txt");
    // pass2("intermediate.txt", argv[2]);

    pass1("source_xe.asm", "intermediate.txt");
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

char* getRegisterAddress(char* operand){
	for(int i=0; i<sizeof(register_name)/sizeof(char*); i++)
		if(!strcmp(register_name[i], operand)) return register_address[i];

	return "0";
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
    if(operand[0] == '#' || operand[0] == '@')
        operand = substring(operand, 1, strlen(operand)-1);
    if(operand[strlen(operand)-1] == ','){
        operand = substring(operand, 0, strlen(operand)-1);
    }

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

int is_indirect(char* operand){
    if (operand == NULL) return 0;
    return (operand[0] == '@');
}

int is_immediate(char* operand){
    if (operand == NULL) return 0;
    return (operand[0] == '#');
}

int is_format4(char* operand){
    if (operand == NULL) return 0;
    return (operand[0] == '+');
}

int is_pc_relative(char* operand){
    if (operand == NULL) return 0;
    
    //if immediate + const
    if(is_immediate(operand)) {
        int intbuf = 0;
        sscanf(operand, "#%d", &intbuf);
        if (intbuf > 0) return 0;
    }
    return 1;
}

int is_register(char* operand){
    return (strlen(operand)==1);
}

int is_format1(char* operand){
    if (operand == NULL) return 0;
    if (strlen(operand) != 1) return 0;
    return (operand[0] == 'A' || 
            operand[0] == 'X' || 
            operand[0] == 'S' || 
            operand[0] == 'B' || 
            operand[0] == 'S' || 
            operand[0] == 'T' || 
            operand[0] == 'L' );
}

int is_format2(char* operand){
    if (operand == NULL) return 0;
    for(int i=0; i<strlen(operand);i++){
        if(operand[i]==',') return 1;
    }
    return 0;
}

void pass2(char* filename, char* output){
	char str[1024] = {};
    char buf[1024];
    char buf2[3];
    char* pbuf[1024] = {};
    int line = 0;
    int base;
    int prev_loc = 0;
    int modi[10] = {};
    int modi_cnt = 0;

	FILE* fp = fopen(filename, "r");
    FILE* fpw = fopen(output, "wt");
    FILE* fpo = fopen("object_code.txt", "wt");

    for(int linenum=1; fgets(str, LINE_BUF_SIZE, fp); linenum++){
        if (isCommentLine(str)) continue; 

        int loc = parseLoc(str);
        if(loc-prev_loc == 0x1000){
            line++;
        }

        prev_loc = loc;
        char** cols = parseLine(str);
		char* label = cols[0];
		char* opcode = cols[1];
		char* operand = cols[2];
		char* operand2 = cols[3];

        if(linenum == 1 && !strcmp(opcode, "START")) {
            sprintf(buf, "H%s  %06X", label, loc);
            fprintf(fpo, "%04X\t%s\t\t%s ", loc, label, opcode);
            sappend(&pbuf[line], buf);
            line++;
            continue;
        }

        fprintf(fpo, "\n%04X\t%s\t\t%s ", loc, label, opcode);
        if(operand) fprintf(fpo, "%s", operand);
        if(operand2) fprintf(fpo, "%s", operand2);

        // break for END operand.
        if(!strcmp(opcode, "END")) break;

        //Make Newline for output
        if(pbuf[line]==NULL || strlen(pbuf[line])==0) {
            sprintf(buf, "T%06X", loc);
            sappend(&pbuf[line], buf);
        }
        if(strlen(pbuf[line]) > 61){
            line++; 
            sprintf(buf, "T%06X", loc);
            sappend(&pbuf[line], buf);
        }

        if(!strcmp(opcode, "BASE")) {
            base = getLocFromSYMTAB(operand);
        }

        char* code = getMachineCode(opcode);
        if(code != NULL){

            // format 1
            if(is_format1(operand)){
                sprintf(buf, "%s%s0", code, getRegisterAddress(operand));
                fprintf(fpo, "\t\t%s%s0", code, getRegisterAddress(operand));
                sappend(&pbuf[line], buf); 
                continue;
            }

            int intcode = strToHex(code)+3; 
            //Write machineCode + operand_address
            int operand_address = getLocFromSYMTAB(operand);            
            if (operand_address > 2) {
                operand_address -= 3;
                if(operand_address < loc) operand_address += 0x1000; // 0x1000 => for 2's complement
                operand_address -= loc;

                // base address / pc relative
                if(loc > base && base == getLocFromSYMTAB(operand)) operand_address = 0x4000;
                else if(is_pc_relative(operand) && strcmp(opcode, "RSUB")) operand_address += 0x2000;
            }

            // indirect / immediate
            if(is_indirect(operand)) intcode -= 1;
            if(is_immediate(operand)) {
                if(operand != NULL){
                    int intbuf = 0;
                    sscanf(operand, "#%d", &intbuf);
                    operand_address += intbuf;
                }
                intcode -= 2;
            }
            
            sprintf(buf2, "%02X", intcode);
            code = buf2;

            // format 2
            if(is_format2(operand)){
                char* operand1 = substring(operand, 0, strlen(operand)-1);
                if(is_register(operand1)){
                    sprintf(buf, "%0X%s%s", intcode-3, getRegisterAddress(operand1), getRegisterAddress(operand2));
                    fprintf(fpo, "\t\t%0X%s%s", intcode-3, getRegisterAddress(operand1), getRegisterAddress(operand2));
                }else{
                    operand_address = getLocFromSYMTAB(operand);
                    operand_address += 0xC000;
                    sprintf(buf, "%0X%04X", intcode, operand_address-base);
                    fprintf(fpo, "\t\t%0X%04X", intcode, operand_address-base);
                }
                sappend(&pbuf[line], buf); 
                continue;
            }

            // format 4
            if(is_format4(opcode)){
                operand_address = getLocFromSYMTAB(operand);
                if(is_immediate(operand)) {
                    int intbuf = 0;
                    sscanf(operand, "#%d", &intbuf);
                    operand_address =+ intbuf;
                }else{
                    //modi locations.
                    modi[modi_cnt++] = (strlen(pbuf[line])-5)/2+(line-1)*29;
                }
                sprintf(buf, "%s1%05X", code, operand_address);
                fprintf(fpo, "\t\t%s1%05X", code, operand_address);
                sappend(&pbuf[line], buf);   
            }else{
                //format 3
                sprintf(buf, "%s%04X", code, operand_address);
                fprintf(fpo, "\t\t%s%04X", code, operand_address);
                sappend(&pbuf[line], buf);   
            }

        }else if(!strcmp(opcode, "WORD")){
            char* b;
            sscanf(operand, "%d", &b);
            sprintf(buf, "%06X", b);
            fprintf(fpo, "\t\t%06X", b);
            sappend(&pbuf[line], buf);

		}else if(!strcmp(opcode, "BYTE")){
             if(operand[0] =='C'){
                char* sub = substring(operand, 2, strlen(operand)-3); // Parse C'EOF' to EOF

                fprintf(fpo, "\t\t");
                for(int i=0;i<strlen(sub);i++){
                    sprintf(buf, "%X", sub[i]);
                    fprintf(fpo, "%X", sub[i]);
                    sappend(&pbuf[line], buf);
                }
            }else if(operand[0] =='X'){
                sprintf(buf, "%s", substring(operand, 2, 2));
                fprintf(fpo, "\t\t%s", substring(operand, 2, 2));
                sappend(&pbuf[line], buf);
            }

        }else if(!strcmp(opcode, "RESW")){
        }else if(!strcmp(opcode, "RESB")){
        }else{
            // printf("--------else!-----\n");
        };
    }

    //Insert File length to each line.
    for(int i=1; pbuf[i]!=NULL;i++){
        sprintf(buf, "%02lX", (strlen(pbuf[i])-7)/2);
        sinsert(&pbuf[i], 7, buf);
    }

    //Write modi record
    for(int i=0; modi[i]!=NULL ;i++){
        sprintf(buf, "M%06X05", modi[i]); //format4 address length is 5.
        sappend(&pbuf[++line], buf);
    }

    //Write end record
    sprintf(buf, "E%s", substring(pbuf[0], 7, 7));
    sappend(&pbuf[line+1], buf);

    //Write filesize at header record.
    sprintf(buf, "%06s", substring(str, 2, strlen(str)-2));
    sappend(&pbuf[0], buf);

    //Print console/file all line buf
    for(int i=0; pbuf[i]!=NULL ;i++){
        printf("%s\n", pbuf[i]);
        fprintf(fpw, "%s\n", pbuf[i]);
    }

    fclose(fp);
    fclose(fpw);
    fclose(fpo);
}