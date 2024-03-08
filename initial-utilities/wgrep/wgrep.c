// wgrep.c file by Kara Fong for CSC 139 Initial Utilities assignment
// Program takes command line arguments consisting of a
// search term and optional file names and then prints out lines 
// containing the indicated search term
// Usage: wgrep <searchTerm> [fileName1] [fileName2] ...


#define _GNU_SOURCE
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){ 
    FILE* in = stdin;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;  

    // search term is missing so print usage statement
    if(argc == 1){
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }
    
    // no files specified, so search stdin
    if(argc == 2){
    	if(in != NULL){    
            // as long as getLine returns a line, check the line
            // for matching characters 
            while ((nread = getline(&line, &len, in)) != -1){
                //if(findMatch2(line, len, argv[1]))
                if(strstr(line, argv[1]))
                    printf("%s", line);
            }
            free(line);
        }
    }

    // for each specified text file, open the file, read the lines, 
    // and print them out. 
    if(argc >= 3){
	    for (int i = 2; i < argc ;i++){
            in = fopen(argv[i], "r");
            if(in != NULL){    
                // as long as getLine returns a line, check the line
                // for matching characters 
                while ((nread = getline(&line, &len, in)) != -1){
                    if(strstr(line, argv[1]))
                        printf("%s", line);		   
                }
            } else{
                printf("wgrep: cannot open file\n");
                exit(1);
            }
            free(line);
            fclose(in);
	    }
    }
    return(0); 
}



