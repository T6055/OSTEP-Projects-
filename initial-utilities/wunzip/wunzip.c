// wunzip.c file by Kara Fong for CSC 139 Initial Utilities assignment
// Program decompresses specified files
// Usage: wunzip <fileName> [fileName2...]

#include<stdio.h> 
#include <stdlib.h>

int main(int argc, char *argv[]){ 
    FILE* in = stdin;
    // no file spoecified
    if(argc == 1){
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }
    
    // for each specified text file, open the file, 
    // decompress the contents, and print them. 
    if(argc > 1){
        // rember the characters and count between files
        int count = 0;
        char cur = 0;
        //char prev = 0; 
		for (int i = 1; i < argc ;i++){
			in = fopen(argv[i], "r");
			// make sure the file opens correctly
			if(in != NULL){    	
				while(1){
                    if(fread(&count, 1, 4, in) != 0){
                        fread(&cur, 1, 1, in);
                        // print out each character the 
                        // specified number of times
                        for(int j = 0; j < count; j++)
                            printf("%c",cur);
                    }else{
                        break;
                    }
				}
			}
            // if the file does not open correctly print 
            // an error statement
            else{
				printf("unwzip: cannot open file\n");
				exit(1);
			}
			fclose(in);
		}
    }
    return(0); 
}






