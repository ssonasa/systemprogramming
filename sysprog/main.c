#include <stdio.h>

int main() {

	FILE* fp;
	char * line = NULL;
	size_t len = 0;

	fp = fopen("FIGURE2-5(TAB).txt", "r");
	// while (!feof(fp)) {
		// fscanf(fp, "%s", &str);
		// printf("%s\n", str);
	// }
	while (getline(&line, &len, fp) != -1) {
        printf("%s", line);
    }


    return 0;
}

