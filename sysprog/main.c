#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define LINE_BUF_SIZE 1024

// Predefine functions, variables.
void file_open();
void printMap();
char *asmMap[][10] = {};

int main() {
	file_open("FIGURE2-5(TAB).txt");
	printMap();
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
	int linenum = 0;
	FILE* fp = fopen(filename, "r");

	while(fgets(line, LINE_BUF_SIZE, fp)){
		int i=0;
		if (isBlankLabel(line)) asmMap[linenum][i++] = "\t";
		if (isCommentLine(line)) continue;

		char* ch = strtok(line, " 	\n");
		for(; ch != NULL; i++){
			asmMap[linenum][i] = (char*) malloc(strlen(ch));
			strcpy(asmMap[linenum][i], ch);
			ch = strtok(NULL, " 	\n");
		}
		linenum++;
	}

	fclose(fp);
}

void printMap(){
	for(int i=0;asmMap[i][0] != NULL; i++){
		for(int j=0; asmMap[i][j]!=NULL; j++){
			if(asmMap[i][j]!=NULL)
				printf("%s/", asmMap[i][j]);
		}
		printf("\n");
	}
}