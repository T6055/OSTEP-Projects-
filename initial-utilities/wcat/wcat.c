// wcat.c file by Kara Fong for CSC 139 Initial Utilities assignment
// Program takes command line arguments consisting of file
// names and then prints out the contents of the files
// Usage: wcat [filename1] [filename2] [filename3] ...

#include<stdio.h> 
#include <stdlib.h>

#define MAX_LINE_LENGTH (512)
#define MAX_LINES (1000000)

int main(int argc, char *argv[]){ 
    FILE* in = stdin;
    char str[MAX_LINE_LENGTH];

    // for each specified text file, open the file, read the lines, and print them out. 
    for (int i = 1; i < argc ;i++){
        in = fopen(argv[i], "r");
        if(in != NULL){     
            while (fgets(str, MAX_LINE_LENGTH, in) != NULL)
                printf("%s", str);
        } else{
            printf("wcat: cannot open file\n");
            exit(1);
        }
        fclose(in);
    }
    return(0); 
}

