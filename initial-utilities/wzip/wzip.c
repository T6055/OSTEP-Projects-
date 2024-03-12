#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	
	int count = 0;
	char current = 0;
	char prev = 0;

	FILE* in = stdin; 
	// the user only entered "wzip" and did not specify a file
	if (argc < 2){  
		// print usage statement
		printf("wzip: file1 [file2 ...]\n");
		// exit(1) meaning that the program run was considered
		// a failure
		exit(1); 
	}

	// if the user entered "wzip" and at least 1 file name
	if (argc > 1){
		// this is to iterate over all the files the user specified
		for (int i = 1; i < argc; i++){ 
			in = fopen(argv[i], "r");
			// if the the file, in, is NULL, then the file did not open correctly
			if (in == NULL) { 
				printf("file null\n");
        	    printf("wzip: cannot open file\n");
				exit(1);		
        	} 
        	// the file was opened correctly so we need to read from it. 
        	else  { 
        		// fread returns the number of items it read from the file
        		while (fread(&current, 1, 1, in) == 1) {
        			// if prev ==0 then, prev has not been initialized to a char yet
	          		if(prev == 0){
	          			prev = current;
	            		count++;

	            	}
	            	// if the current char is the same as the previous char
	            	// then increase the counter for that character
	            	else if (current == prev) {

	            		count ++;
	            	}
	            	else{
						fwrite(&count, 1, 4, stdout); // writes the count value	
                		fwrite(&prev, 1, 1, stdout); // writes the prev value 
                		count = 1; // resets the count to 1 
                		prev =current; // change the previous char to the new char

	            	}
				}
				// when the while loop ends, the last char still needs to be printed out
				if(i == argc-1){
					fwrite(&count, 1, 4, stdout); // writes the count value	
			        fwrite(&prev, 1, 1, stdout); // writes the prev value 
				}
			}
			// close the file
			fclose(in);


		}
	}
}
