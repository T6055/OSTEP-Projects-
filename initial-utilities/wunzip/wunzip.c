#include<stdio.h> 
#include <stdlib.h>

int main(int argc, char *argv[]) { 
    
    int count = 0;
    char current = 0;
    
    FILE* in = NULL;
    
    // no file specified here 
    if (argc == 1) { 
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }

    // iterate over all the files 
    for (int i = 1; i < argc; i++) { 
        // open the current file
        in = fopen(argv[i], "r");
        // this will make sure the file is opened correctly
        if (in == NULL) { 
            printf("wunzip: cannot open file\n");
            exit(1);        
        } 

        // reads from the file
        while(1) {
            if (fread(&count, 1, 4, in) != 0) {
              fread(&current, 1, 1, in);
              // print the character 'count' times
              for (int j = 0; j < count; j++) {
                  printf("%c", current);
              }
            } else {
              // break out of the loop if no more data to read  
              break; 
            }
        }

        // closes the file
        fclose(in);
    }

    return 0;
}
