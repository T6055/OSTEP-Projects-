#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
	FILE* in = stdin;

	//char myString[512];

	//char myString[512];
	char line[512];

	
	if (argc == 1){ // this is searching 
		printf("wgrep: searchterm [file ...]\n");
		
		exit(1);
	}

	if (argc == 2){ // stdin is the default file pointer, and this will search it  
		if(in != NULL){ //check to see if stdin is null and if not then loop through and output whatever is in stdin (stdin is a place holder for fgets) dont need a for loop bc it doesnt have like 3 arguements
			while (fgets(line,512, stdin) != NULL){ // whatever is in fgets will go into stdin, 
				if(strstr(line, argv[1]))
					printf("%s", line);
			}
		}
	}
 	else if (argc >= 3) {

 		//char myString[512] = argv[1];
	
		for (int i = 2; i < argc; i++){ // used for the argv and argc stuff
			in = fopen(argv[i], "r");
			if (in == NULL) { // to check if the file opened correctly, if not then we would get NULL (nothing) == means equal
			    printf("wgrep: cannot open file\n");
			    exit(1);
				
			}
			if(in != NULL){ //check to see if stdin is null and if not then loop through and output whatever is in stdin (stdin is a place holder for fgets) dont need a for loop bc it doesnt have like 3 arguements
				while (fgets(line,512, in) != NULL){ // whatever is in fgets will go into stdin, 
					if(strstr(line, argv[1]))
						printf("%s", line);
				}
			}
			
		}
	}  
	fclose(in);
	return 0;
	
}
	