#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


static char** allocate_matrix(int nrows, int ncols) 
{
    int i;
    char **matrix;

    /*  allocate array of pointers  */
    matrix = malloc( nrows*sizeof(char*));

    if(matrix==NULL)
        return NULL; /* Allocation failed */

    /*  Allocate column for each name  */
    for(i = 0; i < nrows; i++)
        matrix[i] = malloc( ncols*sizeof(char));

    if(matrix[i-1] == NULL) 
        return NULL; /* Allocation failed */

    return matrix;
}


int main(){
    char* opcodeStr[] = {"ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP", "COMPF", "COMPR", "DIV", "DIVF",
                        "DIVR", "FIX", "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT", "JSUB", "LDA",
                        "LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
                        "MULR", "NORM", "OR", "RD", "RMO", "RSUB", "SHIFTL", "SHIFTR", "SIO", "SSK",
                        "STA", "STB", "STCH", "STF", "STI", "STL", "STS", "STSW", "STT", "STX",
                        "SUB", "SUBF", "SUBR", "SVC", "TD", "TIO", "TIX", "TIXR", "WD"};

	// char* machinecodeStr = "18/58/90/40/B4/28/88/A0/24/64/9C/C4/C0/F4/3C/30/34/38/48/00/68/50/70/08/6C/74/04/D0/20/60/98/C8/44/D8/AC/4C/A4/A8/F0/EC/0C/78/54/80/D4/14/7C/E8/84/10/1C/5C/94/B0/E0/F8/2C/B8/DC";
	
    // for(int i=0; i<10; i++){
    //     printf("%s", opcodeStr[i]);
	//     // char* ch = strtok(opcodeStr, "/");
    // }
    char** matrix = allocate_matrix(10, 10);
    printf("%s", matrix[0);

    return 0;
}