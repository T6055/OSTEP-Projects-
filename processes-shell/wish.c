#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char *searchPaths[100] = {"/bin/", "/usr/bin/"};
int pathCount = 2;
char *commandTokens[100];
int tokenCount = 0; 
int processIDs[100];
int processCount = 0;

void splitInputIntoTokens(FILE *source); 
void processNextCommand(); 
int checkForRedirection(char *tokens[], int startingIndex); 
void displayError(); 

int main(int argc, char *argv[]) {
    switch(argc) {
        case 1: // Interactive mode
            splitInputIntoTokens(stdin);
            break;
        case 2: { // Batch mode
            FILE *fileStream = fopen(argv[1], "r");
            if(fileStream) {
                splitInputIntoTokens(fileStream);
                fclose(fileStream); // Properly close the file
            } else {
                displayError();
                exit(EXIT_FAILURE);
            }
            break;
        }
        default: // Incorrect number of arguments
            displayError();
            exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

/*void splitInputIntoTokens(FILE *source)
{
    while (1)
    {
        if (source == stdin)
            printf("wish> ");
        int prevTokenCount = tokenCount;
        tokenCount = 0;
        char *line = NULL;
        size_t len = 0;
        if (getline(&line, &len, source) == EOF)
            exit(0);
        size_t lineSize = strlen(line);
        char *adjustedLine = malloc(lineSize * 6);
        int adjustment = 0;
        if (!strcmp(line, "&\n")) continue;
        for (int i = 0; i < lineSize; i++)
        {
            if (line[i] == '>' || line[i] == '&' || line[i] == '|') 
            {
                adjustedLine[i + adjustment] = ' ';
                adjustment++;
                adjustedLine[i + adjustment] = line[i];
                i++;
                adjustedLine[i + adjustment] = ' ';
                adjustment++;
            }
            adjustedLine[i + adjustment] = line[i];
        }
        char *token;
        while ((token = strsep(&adjustedLine, " \n")) != NULL)
        {
            if (!strcmp(token, "&")) 
            {
                commandTokens[tokenCount] = NULL;
                tokenCount++;
            }
            if (memcmp(token, "\0", 1))
            {
                commandTokens[tokenCount] = token;
                tokenCount++;
            }
        }
        for (int i = tokenCount; i < prevTokenCount; i++)
        {
            commandTokens[i] = NULL;
        }
        processNextCommand(); 
        free(adjustedLine);
        for (int i = 0; i < processCount; i++) 
        {
            int status;
            waitpid(processIDs[i], &status, 0);
        }
    }
}
*/

 void splitInputIntoTokens(FILE *source) {
     char *line = NULL;
     size_t len = 0;
     ssize_t read;

     while ((read = getline(&line, &len, source)) != -1) {
         if (source == stdin) {
             printf("wish> ");
         }

         if (!strcmp(line, "&\n")) continue;  //Skip empty commands

         char *lineCopy = strdup(line);  //Duplicate the line for safe manipulation
         char *token, *rest = lineCopy;
         tokenCount = 0;  //Reset token count for each new line

         while ((token = strtok_r(rest, " \n", &rest))) {
             if (token[0] == '>' || token[0] == '&' || token[0] == '|') {
                 commandTokens[tokenCount++] = " ";  //Separate operators from commands/arguments
             }
             commandTokens[tokenCount++] = token;
         }

         processNextCommand(); 

         free(lineCopy);  //Free the duplicated line
     }
    
     free(line);  //Free the original line allocated by getline
     for (int i = 0; i < processCount; i++) {
         int status;
         waitpid(processIDs[i], &status, 0);
     }
 }


void processNextCommand()
{
    for (int i = 0; i < tokenCount; i++)
    {
        if (!strcmp(commandTokens[i], "exit"))
        {
            if (commandTokens[i + 1] == NULL)
            {
                exit(0);
            }
            else
            {
                i++;
                displayError();
            }
        }
        else if (!strcmp(commandTokens[i], "cd"))
        {
            if (chdir(commandTokens[++i])) 
            {
                displayError();
            }
            while (commandTokens[i] != NULL) i++;
            i++;
        }
        else if (!strcmp(commandTokens[i], "path"))
        {
            pathCount = 0;
            for (int j = 1; j < tokenCount; j++)
            {
                char *resolvedPath = realpath(commandTokens[j], NULL); 
                if (resolvedPath != NULL)
                    searchPaths[j - 1] = resolvedPath;
                else 
                    displayError();
                pathCount++;
            }
            i += pathCount;
        }
        else 
        {
            int pid = fork();
            processIDs[processCount++] = pid;
            if (pid == -1)
            {
                displayError();
                exit(1);
            }
            if (pid == 0) 
            {
                if (checkForRedirection(commandTokens, i) != -1) 
                {
                    for (int j = 0; j < pathCount; j++)
                    {
                        char *fullPath = malloc(4096); 
                        strcpy(fullPath, searchPaths[j]);
                        strcat(fullPath, "/");
                        strcat(fullPath, commandTokens[i]); 
                        if (!access(fullPath, X_OK))
                        {
                            execv(fullPath, commandTokens + i); 
                        }
                    }
                    displayError();
                    exit(1);
                }
            }
            else
            {
                while (commandTokens[i] != NULL) i++;
                i++;
                if (i < tokenCount)
                    continue;
            }
        }
    }
}

int checkForRedirection(char *tokens[], int start)
{
    for (int i = start; tokens[i] != NULL; i++)
    {
        if (!strcmp(tokens[i], ">") && i != start) 
        {
            tokens[i] = NULL;
            if (tokens[i + 1] == NULL || tokens[i + 2] != NULL) 
            {
                displayError();
                return -1;
            }
            else 
            {
                int fileDescriptor = open(tokens[i + 1], O_WRONLY | O_CREAT, 0777);
                dup2(fileDescriptor, STDOUT_FILENO); 
                tokens[i + 1] = NULL;
                close(fileDescriptor);
            }
            return i;
        }
    }
    return 0;
}

void displayError()
{
    char errorMessage[30] = "An error has occurred\n";
    write(STDERR_FILENO, errorMessage, strlen(errorMessage));
}
