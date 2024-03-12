#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERROR_MESSAGE_LENGTH 30
#define MAX_TOKENS 64
#define MAX_PATHS 100

// Define processIDs and processCount
pid_t processIDs[MAX_PATHS];
int processCount = 0;

// Declare getPath if not defined elsewhere
char *getPath(char *arg);

void userloop();
void batchloop(FILE *file);
int wishexit(char **args);
int wishExecute(char **args);
int wishnumbuiltins();
char **tokenize(char *line, char *delim);
void changeDirectory(char *path);
int wishPath(char **args);
int wishcd(char **args);
int wishLaunch(char **args);
char *concatPath(const char *path1, const char *path2);

// Function declarations
int validateArgs(char **args);
char *getPath(char *arg);

char *builtinstr[] = {"cd", "exit", "path"};
int (*builtinfunc[])(char **) = {&wishcd, &wishexit, &wishPath};
char error_message[ERROR_MESSAGE_LENGTH] = "An error has occurred\n";
char *path[MAX_PATHS]; // Define path as an array of pointers
int pathNull = 0;
int paths = 0;

int main(int argc, char *argv[])
{
    FILE *file;
    if (argc > 2)
    {
        fprintf(stderr, "%s", error_message);
        exit(EXIT_FAILURE);
    }
    else
    {
        if (argc == 2)
        {
            file = fopen(argv[1], "r");
            if (file != NULL)
            {
                batchloop(file);
            }
        }
        else if (argc == 1)
            userloop();
    }
}

void userloop()
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while (1)
    {
        printf("wish> ");
        nread = getline(&line, &len, stdin);
        if (nread == -1)
        {
            exit(0);
        }
        else
        {
            wishExecute(tokenize(line, " \t\n"));
        }
    }
    free(line);
}

void batchloop(FILE *file)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    nread = getline(&line, &len, file);

    if (nread != -1)
    {
        do
        {
            if (strcmp(line, "exit") == 0 || strcmp(line, "exit\n") == 0)
            {
                exit(0);
            }

            wishExecute(tokenize(line, " \t\n"));

            nread = getline(&line, &len, file);
        } while (nread != -1);
    }

    free(line);
    exit(0);
}

int wishcd(char **args){
    // no path specified
    if(args[1] == NULL){
            fprintf(stderr, "%s", error_message);
    }
    else{
        // Use args[1] directly assuming it contains the path
        // path was invalid
        if(chdir(args[1]) != 0){
            fprintf(stderr, "%s", error_message);
            return 1;
        }  
    }
    return 0;
}

void changeDirectory(char *path)
{
    int rc = chdir(path);
    if (rc != 0)
    {
        fprintf(stderr, "%s", error_message);
    }
}

int wishexit(char **args) {
    int i = 1;
    while (args[i] != NULL) {
        fprintf(stderr, "%s", error_message);
        return 0;
        i++;
    }
    exit(0);
}


int wishExecute(char **args)
{
    if (args[0] == NULL)
        return 1;

    // Check if the command is a built-in command
    for (int i = 0; i < wishnumbuiltins(); i++)
    {
        if (strcmp(args[0], builtinstr[i]) == 0)
        {
            return (*builtinfunc[i])(args); // Execute built-in command
        }
    }

    // If the command is not a built-in command, execute it
    char binPath[256]; // Assuming max path length of 256
    sprintf(binPath, "/bin/%s", args[0]); // Prepend /bin/ to the command name

    // Execute the command
    if (execv(binPath, args) == -1)
    {
        //perror("execv"); // Print error if execv fails
        fprintf(stderr, "%s", error_message);
    }

    // This line will only be reached if execv fails
    return 0;
}

int wishnumbuiltins()
{
    return sizeof(builtinstr) / sizeof(char *);
}

char **tokenize(char *line, char *delim)
{
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    char *token;
    int index = 0;

    token = strtok(line, delim);
    while (token != NULL)
    {
        tokens[index] = token;
        index++;
        token = strtok(NULL, delim);
    }
    tokens[index] = NULL;
    return tokens;
}

int wishPath(char **args)
{
    if (args[1] == NULL)
    {
        pathNull = 1;
        return 0;
    }
    for (int i = 1; args[i] != NULL; i++)
    {
        if (args[i] != NULL)
        {
            char *tmp = (char *)malloc(200 * sizeof(char));
            getcwd(tmp, 200);
            if (!access(args[i], X_OK))
            {
                // found the path in the cwd
                // add the current working directory so it's an absolute path
                path[paths] = concatPath(tmp, args[i]);
                paths++;
                // a path has been specified.
                pathNull = 0;
            }
        }
    }

    return 0;
}

int wishLaunch(char **args) {
    int i = 0;
    pid_t pid;
    
    while (args[i] != NULL) {
        pid = fork();
        processIDs[processCount] = pid;
        processCount++;
        
        if (pid == 0) {
            // Child process
            if (validateArgs(args)) {
                fprintf(stderr, "%s", error_message);
                exit(EXIT_FAILURE);
            } else {
                char *cmdpath = getPath(args[i]);
                if (*cmdpath != '\0') {
                    execv(cmdpath, args);
                } else {
                    fprintf(stderr, "%s", error_message);
                    exit(EXIT_FAILURE);
                }
            }
        } else if (pid < 0) {
            fprintf(stderr, "%s", error_message);
        }
        
        i++;
    }
    
    return pid;
}

char *concatPath(const char *path1, const char *path2) {
    size_t len1 = strlen(path1);
    size_t len2 = strlen(path2);

    // Allocate memory for the concatenated path
    char *result = malloc(len1 + len2 + 2); // Plus 2 for '/' and '\0'
    if (result == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Copy path1 and path2 into the result buffer
    strcpy(result, path1);
    strcat(result, "/");
    strcat(result, path2);

    return result;
}

char* getPath(char *s1) {
    char *s2;
    char *result = malloc(100 * sizeof(char));
    int i = 0;
    while (i < paths) {
        s2 = path[i];
        result = concatPath(s2, s1);
        if (!access(result, X_OK)) {
            return result;
        }
        i++;
    }
    // Corrected to use getcwd instead of printcwd
    s2 = getcwd(NULL, 0);
    result = concatPath(s2, s1);
    if (!access(result, X_OK)) {
        return result;
    }
    result[0] = '\0'; // Setting the first character to null terminator
    return result;
}

int validateArgs(char **args) {
    // This function should validate if the arguments are valid.
    // For the sake of this example, let's assume they are always valid.
    return 0;
}
