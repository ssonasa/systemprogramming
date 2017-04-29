#include <stdio.h>

int main(void) {
	FILE *fpt;
	char label[100];

	fpt = fopen("FIGURE52-5(TAB).txt", "r");

	while (!feof(fpt))
	{
		fgets(label, 100, fpt);
		puts(label);

	}

	fclose(fpt);
	return 0;
}