#include <stdio.h>

int main() {

	FILE* fp;
	char str[100];
	fp = fopen("FIGURE2-5(TAB).txt", "r");
	while (!feof(fp)) {
		fscanf(fp, "%s", &str);
		printf("%s\n", str);
	}

    return 0; //test
}

