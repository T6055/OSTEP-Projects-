#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) { //we have these for command line arguements, argv is for actual strings that come after the call, and argc is the number of strings seperated by a space argv v is to store the name of the file so wcat taeler argv is to command wcat show you taeler file. 
	FILE* in = stdin;
	
	for (int i = 1; i < argc; i++) { // argc is the # of loops you want it to go through, and i = 1 is where you want to start which is going to be test.txt in this assignment. 
		in = fopen(argv[i], "r");
		
		if (in == NULL) {// to check if the file opened correctly, if not then we would get NULL (nothing) == means qual
			printf("wcat: cannot open file\n");
			
			exit (1);
		}
		char myString[512];
		while (fgets(myString, 512, in) != NULL) { //!= means not equal 
			// Print the file content
			printf("%s", myString);
		}
			
	}
	 
	fclose(in);
}