#include <stdio.h>
#include <string.h>
#include <ctype.h>

void file_open();

char *asmMap[51][4] = {};

int main() {
	file_open();
    return 0;
}


void file_open(){
	FILE* fp;
	size_t len = 0;
	int linenum = 0;

	fp = fopen("FIGURE2-5(TAB).txt", "r");
	int buf_size = 1024;
	char line[buf_size];
	while(fgets(line, buf_size, fp)) {

		char *ch;
		int i=0;

		if (isspace(line[0])){
			asmMap[linenum][0] = NULL;
			i++;
		}
		

		ch = strtok(line, " 	\n");
		while (ch != NULL) {
			i++;
			asmMap[linenum][i] = ch;
			// printf("%s", asmMap[linenum][i]);
			// printf("%s/", ch);
			ch = strtok(NULL, " 	\n");
		}

		// printf("%d\n", i);
		// printf("%d", linenum);
		linenum++;
	}

	for(int i=0; i<10; i++){
		for(int j=0; j<4; j++){
			printf("%s	", asmMap[i][j]);
		}
		printf("\n");
	}
}