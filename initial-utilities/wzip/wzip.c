// wzip.c file by Kara Fong for CSC 139 Initial Utilities assignment
// Program compresses specified files
// Usage: wzip <fileName> [fileName2...]

#include<stdio.h> 
#include <stdlib.h>

int main(int argc, char *argv[]){ 
    FILE* in = stdin;
    // no file spoecified
    if(argc == 1){
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }
    
    // for each specified text file, open the file, read the characters, 
    // and compress them. 
    if(argc > 1){
        // rember the characters and count between files
        int count = 1;
        char cur = 0;
        char prev = 0; 
		for (int i = 1; i < argc ;i++){
			in = fopen(argv[i], "r");
			// make sure the file opens correctly
			if(in != NULL){    	
				while(1){
					cur = fgetc(in); 
					if(cur == EOF){
                        // print the previous character if the end of the last file is reached
						if (count >= 1 && i == argc-1){     						
                            fwrite(&count, 1, 4, stdout);
                            fwrite(&prev, 1, 1, stdout);
						}
						break;
                    }
                    // when there is a different char, print out the previous one
                    else if (cur !=  prev){
                        if(prev == 0){
                            prev = cur;
                            continue;
                        }else{
                            fwrite(&count, 1, 4, stdout);
                            fwrite(&prev, 1, 1, stdout);
                            count = 1;
                            prev =cur;
                        }
                    // if characters are the same, increase the count
					}else if( cur == prev){
                            count++;
                    }
				}
			}
            // if the file does not open correctly 
            else{
				printf("wzip: cannot open file\n");
				exit(1);
			}
			fclose(in);
		}
    }
    return(0); 
}






