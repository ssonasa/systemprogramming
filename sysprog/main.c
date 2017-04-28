#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define LINE_BUF_SIZE 1024

void file_open();
void printAsmMap();
char *asmMap[51][4] = {};

int main() {
	file_open("FIGURE2-5(TAB).txt");
	printAsmMap();
    return 0;
}

int isBlankLabel(char* str){
	return isspace(str[0]);
}

int isCommentLine(char* str){
	return str[0] == '.';
}

void file_open(char* filename){
	char line[1024];
	FILE* fp = fopen(filename, "r");

	for(int linenum=0; fgets(line, LINE_BUF_SIZE, fp); linenum++){
		int i=0;
		if (isBlankLabel(line)) asmMap[linenum][i++] = "\t";
		if (isCommentLine(line)) continue;


		char* ch = strtok(line, " 	\n");
		for(; ch != NULL; i++){
			asmMap[linenum][i] = (char*) malloc(strlen(ch));
			strcpy(asmMap[linenum][i], ch);
			ch = strtok(NULL, " 	\n");
		}
	}
	fclose(fp);
}

void printAsmMap(){
	for(int i=0; i<51; i++){
		for(int j=0; j<4; j++){
			if(asmMap[i][j]!=NULL)
				printf("%s/", asmMap[i][j]);
		}
		printf("\n");
	}

}