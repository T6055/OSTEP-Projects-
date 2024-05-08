#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_STRING 4096

// Define a static struct to hold the options
static struct {
    int human;
    int all;
    int format;
    int json;
    int log;
    int inode;
    int recursive;
    char* logPath;
    char* path;
} Options = {0, 0, 0, 0, 0, 1, 0, NULL, NULL}; // Initialize all options to default values

void print_console_Output(struct stat fileInfo, char** argv);
void print_JSON_Output(const char*, const char*, const char*, const char*, const char*, const char*,
                       const char*, const char*, const char*, const char*, const char*);
void help();
int validate_file(struct stat *fileInfo);
char* getNumber(struct stat);
char* getType(struct stat);
char* getPermissions(struct stat);
char* getLinkCount(struct stat);
char* getUid(struct stat);
char* getGid(struct stat);
char* getSize(struct stat);
char* getAccessTime(struct stat, int human);
char* getModTime(struct stat, int human);
char* getStatusChangeTime(struct stat fileInfo, int human);
void parseargs(int argc, char *argv[]);
int shortArgs(char option);
int longArgs(char* opt);
void errorOption(char*);



int main(int argc, char *argv[]) {
    struct stat fileInfo;
    
    if (argc <= 1) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]); //error message too much stuff happening in command
        return 1;
    }

    
    
    parseargs(argc, argv); // Set all flags and paths

    if (validate_file(&fileInfo) != 0) {  // Check file after parsing args
        return 1;  // Exit if validation fails
    }

    if (Options.json) {
        print_JSON_Output(Options.path, getNumber(fileInfo), getType(fileInfo),
                          getPermissions(fileInfo), getLinkCount(fileInfo), getUid(fileInfo),
                          getGid(fileInfo), getSize(fileInfo), getAccessTime(fileInfo, Options.human),
                          getModTime(fileInfo, Options.human), getStatusChangeTime(fileInfo, Options.human));
    } else {
        print_console_Output(fileInfo, argv);
    }

    return 0;  // Success
}
    
int validate_file(struct stat *fileInfo) {
    if (Options.path != NULL && stat(Options.path, fileInfo) != 0) {
        fprintf(stderr, "Error getting file info for %s: %s\n", Options.path, strerror(errno));
        return 1;  // Return 1 to indicate an error
    }
    return 0;  // Return 0 to indicate success
}

void parseargs(int argc, char **argv) {
    //printf("This is working\n"); //checks to see if it works 
    
    if (argc <= 1) {
        help();
        exit(EXIT_FAILURE);
    }
    int i = 1;
    while (i < argc) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == '-') {  // Handle long options ex: --help 
                longArgs(argv[i]);
                if (Options.log && i + 1 < argc) {
                    Options.logPath = argv[++i]; // Handle log path if necessary
                }
            } else {  // Handle short options
                for (int j = 1; argv[i][j] != '\0'; j++) {
                    shortArgs(argv[i][j]); // Correctly passing the character
                    if (Options.log && i + 1 < argc) {
                        Options.logPath = argv[++i];
                        break; // Exit the loop after handling the log path
                    }
                }
            }
        } else {
            Options.path = argv[i]; // Assume it's a valid path
        }
        i++;
    }

    if (!Options.all && Options.path == NULL) {
        help(); // Show help if no path provided and 'all' is not specified
        exit(EXIT_FAILURE);
    }
}

int shortArgs(char option) {
    switch (option) {
        case '?':
            help();
            return 1;
        case 'i':
            Options.inode = 1;
            return 1;
        case 'a':
            Options.all = 1;
            return 1;
        case 'r':
            if (Options.all) {
                Options.recursive = 1;
                return 1;
            } else {
                fprintf(stderr, "Error: Recursive option '-r' requires '-a' option first.\n");
                exit(EXIT_FAILURE);
            }
        case 'h':
            Options.human = 1;
            return 1;
        case 'f':
            Options.format = 1;
            return 1;
        case 'l':
            Options.log = 1;
            return 1;
        default:
            fprintf(stderr, "Error: Unknown option '-%c'.\n", option);
            exit(EXIT_FAILURE);
    }
    return 0; // Default case if no match found
}

int longArgs(char* opt) {
    if (strcmp(opt, "--help") == 0) {
        help();
        return 1;
    }
    if (strcmp(opt, "--inode") == 0) {
        Options.inode = 1;
        return 1;
    }
    if (strcmp(opt, "--all") == 0) {
        Options.all = 1;
        return 1;
    }
    if (strcmp(opt, "--recursive") == 0) {
        if (Options.all) {
            Options.recursive = 1;
            return 1;
        }
        return 0;
    }
    if (strcmp(opt, "--human") == 0) {
        Options.human = 1;
        return 1;
    }
    if (strcmp(opt, "--format") == 0) {
        Options.format = 1;
        return 1;
    }
    if (strcmp(opt, "--log") == 0) {
        Options.log = 1;
        return 1;
    }
    return 0; // Return 0 if no valid option was matched
}
 
void print_console_Output(struct stat fileInfo, char** argv){
    printf("\nInfo for: %s:\n", argv[1]);
    printf("File Inode: %llu\n", fileInfo.st_ino);
    printf("File Type: ");
    if (S_ISREG(fileInfo.st_mode))
        printf("regular file\n");
    else if (S_ISDIR(fileInfo.st_mode))
        printf("directory\n");
    else if (S_ISCHR(fileInfo.st_mode))
        printf("character device\n");
    else if (S_ISBLK(fileInfo.st_mode))
        printf("block device\n");
    else if (S_ISFIFO(fileInfo.st_mode))
        printf("FIFO (named pipe)\n");
    else if (S_ISLNK(fileInfo.st_mode))
        printf("symbolic link\n");
    else if (S_ISSOCK(fileInfo.st_mode))
        printf("socket\n");
    else
        printf("unknown?\n");

    //get it human readable 
    char* sizeStr = getSize(fileInfo);  
    char* accessTime = getAccessTime(fileInfo, Options.human);
    char* modTime = getModTime(fileInfo, Options.human);
    char* statusChangeTime = getStatusChangeTime(fileInfo, Options.human);

    printf("Number of Hard Links: %hu\n", fileInfo.st_nlink);
    printf("File Size: %s\n", sizeStr);  // Use formatted size
    printf("Last Access Time: %s\n", accessTime);
    printf("Last Modification Time: %s\n", modTime);
    printf("Last Status Change Time: %s\n", statusChangeTime);

    printf("\nChecking to see if the Options Values are changing....\n");
    printf("Options:\n");
    printf("  human: %d\n", Options.human);
    printf("  all: %d\n", Options.all);
    printf("  format: %d\n", Options.format);
    printf("  json: %d\n", Options.json);
    printf("  log: %d\n", Options.log);
    printf("  inode: %d\n", Options.inode);
    printf("  recursive: %d\n", Options.recursive);
    printf("  logPath: %s\n", Options.logPath ? Options.logPath : "NULL");
    printf("  path: %s\n", Options.path ? Options.path : "NULL");

    free(accessTime);
    free(modTime);
    free(statusChangeTime);
}

void print_JSON_Output(const char* path, const char* number, const char* type,
                         const char* permissions, const char* linkCount,
                         const char* uid, const char* gid, const char* size,
                         const char* accessTime, const char* modTime,
                         const char* statusChangeTime){
    static char opt[MAX_STRING]; // Fixed-size character array
    int length = snprintf(opt, MAX_STRING,
       "  {\n"
    "    \"filepath\": \"%s\",\n"
    "    \"inode\": {\n"
    "      \"number\": %s,\n"
    "      \"type\": \"%s\",\n"
    "      \"permissions\": \"%s\",\n"
    "      \"linkCount\": %s,\n"
    "      \"uid\": %s,\n"
    "      \"gid\": %s,\n"
    "      \"size\": %s,\n"
    "      \"accessTime\": %s,\n"
    "      \"modificationTime\": %s,\n"
    "      \"statusChangeTime\": %s\n"
    "    }\n"
    "  },\n",
        path, number, type, permissions, linkCount, uid,
        gid, size, accessTime, modTime, statusChangeTime);
    if (length >= MAX_STRING) {
        // Handle overflow error here
        // This implementation does not support handling overflow
    }
    printf("%s", opt);
    printf("\nOptions:\n");
    printf("  human: %d\n", Options.human);
    printf("  all: %d\n", Options.all);
    printf("  format: %d\n", Options.format);
    printf("  json: %d\n", Options.json);
    printf("  log: %d\n", Options.log);
    printf("  inode: %d\n", Options.inode);
    printf("  recursive: %d\n", Options.recursive);
    printf("  logPath: %s\n", Options.logPath ? Options.logPath : "NULL");
    printf("  path: %s\n", Options.path ? Options.path : "NULL");


}
char* getNumber(struct stat fileInfo) {
    static char buffer[20];
    snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)fileInfo.st_ino);
    return buffer;
}

char* getType(struct stat fileInfo) {
    static char type[20]; // Assuming the type won't exceed 20 characters
    const char* types[] = {
        "regular file",
        "directory",
        "character device",
        "block device",
        "FIFO",
        "symbolic link",
        "socket"
    };
    mode_t mode = fileInfo.st_mode;
    int i = 0;
    while (i < 7) {
        if (S_ISREG(mode)) {
            strcpy(type, types[0]);
            break;
        } else if (S_ISDIR(mode)) {
            strcpy(type, types[1]);
            break;
        } else if (S_ISCHR(mode)) {
            strcpy(type, types[2]);
            break;
        } else if (S_ISBLK(mode)) {
            strcpy(type, types[3]);
            break;
        } else if (S_ISFIFO(mode)) {
            strcpy(type, types[4]);
            break;
        } else if (S_ISLNK(mode)) {
            strcpy(type, types[5]);
            break;
        } else if (S_ISSOCK(mode)) {
            strcpy(type, types[6]);
            break;
        } else {
            strcpy(type, "unknown");
            break;
        }
        i++;
    }
    return type;
}

char* getPermissions(struct stat fileInfo) {
    static char str1[11];
    int i = 0;
    str1[i++] = (S_ISDIR(fileInfo.st_mode)) ? 'd' : '-';
    str1[i++] = (fileInfo.st_mode & S_IRUSR) ? 'r' : '-';
    str1[i++] = (fileInfo.st_mode & S_IWUSR) ? 'w' : '-';
    str1[i++] = (fileInfo.st_mode & S_IXUSR) ? 'x' : '-';
    str1[i++] = (fileInfo.st_mode & S_IRGRP) ? 'r' : '-';
    str1[i++] = (fileInfo.st_mode & S_IWGRP) ? 'w' : '-';
    str1[i++] = (fileInfo.st_mode & S_IXGRP) ? 'x' : '-';
    str1[i++] = (fileInfo.st_mode & S_IROTH) ? 'r' : '-';
    str1[i++] = (fileInfo.st_mode & S_IWOTH) ? 'w' : '-';
    str1[i++] = (fileInfo.st_mode & S_IXOTH) ? 'x' : '-';
    str1[i++] = '\0';
    return str1;
}

char* getLinkCount(struct stat fileInfo) {
    static char buffer[20];
    snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)fileInfo.st_nlink);
    return buffer;
}

char* getUid(struct stat fileInfo) {
    static char buffer[20];
    snprintf(buffer, sizeof(buffer), "%u", fileInfo.st_uid);
    return buffer;
}

char* getGid(struct stat fileInfo) {
    static char buffer[20];
    snprintf(buffer, sizeof(buffer), "%u", fileInfo.st_gid);
    return buffer;
}

char* getSize(struct stat fileInfo) {
    printf("this is running i think\n");
    static char buf[64]; // Static buffer for the size string
    long long size = fileInfo.st_size;
    if (Options.human) {
        printf("working!\n");
        static const char *SIZES[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB" }; // Include all units up to exabytes
        int div = 0;

        while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES) - 1) {
            size /= 1024;
            div++;
        }

        if (div == 0) {  // Bytes do not need a decimal
            snprintf(buf, sizeof(buf), "%lld%s", size, SIZES[div]);
        } else {  // Other units get one decimal for precision
            snprintf(buf, sizeof(buf), "%.1f%s", (double)size, SIZES[div]);
        }
    } else {
        snprintf(buf, sizeof(buf), "%lld", size);
    }

    return buf; // Return the buffer containing the formatted size
}



char* getAccessTime(struct stat fileInfo, int human) {
    if (human) {
        struct tm *info;
        // Convert time_t to broken-down time representation
        info = localtime(&fileInfo.st_atime);
        // Calculate the required buffer size for the formatted string
        int n = snprintf(NULL, 0, "\"%04d-%02d-%02d %02d:%02d:%02d\"",
                         info->tm_year + 1900, info->tm_mon + 1,
                         info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);
        assert(n > 0); // Ensure the calculation is successful
        // Allocate memory for the formatted string
        char* formatted_time = malloc(n + 1);
        if (!formatted_time) {
            return NULL; // Return NULL if memory allocation fails
        }
        // Format the date and time into the allocated buffer
        snprintf(formatted_time, n + 1, "\"%04d-%02d-%02d %02d:%02d:%02d\"",
                 info->tm_year + 1900, info->tm_mon + 1,
                 info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);
        return formatted_time;
    } else {
        time_t seconds = fileInfo.st_atime;
        // Calculate the buffer size needed for the timestamp
        int n = snprintf(NULL, 0, "%ld", seconds);
        assert(n > 0); // Ensure the calculation is successful
        // Allocate memory for the timestamp string
        char* formatted_time = malloc(n + 1);
        if (!formatted_time) {
            return NULL; // Return NULL if memory allocation fails
        }
        // Format the timestamp into the buffer
        snprintf(formatted_time, n + 1, "%ld", seconds);
        return formatted_time;
    }
}

char* getModTime(struct stat fileInfo, int human) {
    if (human) {
        struct tm *info;
        // Convert time_t to broken-down time representation
        info = localtime(&fileInfo.st_mtime); // Use st_mtime for modification time
        // Calculate the required buffer size for the formatted string
        int n = snprintf(NULL, 0, "\"%04d-%02d-%02d %02d:%02d:%02d\"",
                         info->tm_year + 1900, info->tm_mon + 1,
                         info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);
        assert(n > 0); // Ensure the calculation is successful
        // Allocate memory for the formatted string
        char* formatted_time = malloc(n + 1);
        if (!formatted_time) {
            return NULL; // Return NULL if memory allocation fails
        }
        // Format the date and time into the allocated buffer
        snprintf(formatted_time, n + 1, "\"%04d-%02d-%02d %02d:%02d:%02d\"",
                 info->tm_year + 1900, info->tm_mon + 1,
                 info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);
        return formatted_time;
    } else {
        time_t seconds = fileInfo.st_mtime; // Use st_mtime for modification time
        // Calculate the buffer size needed for the timestamp
        int n = snprintf(NULL, 0, "%ld", seconds);
        assert(n > 0); // Ensure the calculation is successful
        // Allocate memory for the timestamp string
        char* formatted_time = malloc(n + 1);
        if (!formatted_time) {
            return NULL; // Return NULL if memory allocation fails
        }
        // Format the timestamp into the buffer
        snprintf(formatted_time, n + 1, "%ld", seconds);
        return formatted_time;
    }
}

char* getStatusChangeTime(struct stat fileInfo, int human) {
    if (human) {
        struct tm *info;
        // Convert time_t to broken-down time representation
        info = localtime(&fileInfo.st_ctime); // Use st_ctime for status change time
        // Calculate the required buffer size for the formatted string
        int n = snprintf(NULL, 0, "\"%04d-%02d-%02d %02d:%02d:%02d\"",
                         info->tm_year + 1900, info->tm_mon + 1,
                         info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);
        assert(n > 0); // Ensure the calculation is successful
        // Allocate memory for the formatted string
        char* formatted_time = malloc(n + 1);
        if (!formatted_time) {
            return NULL; // Return NULL if memory allocation fails
        }
        // Format the date and time into the allocated buffer
        snprintf(formatted_time, n + 1, "\"%04d-%02d-%02d %02d:%02d:%02d\"",
                 info->tm_year + 1900, info->tm_mon + 1,
                 info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);
        return formatted_time;
    } else {
        time_t seconds = fileInfo.st_ctime; // Use st_ctime for status change time
        // Calculate the buffer size needed for the timestamp
        int n = snprintf(NULL, 0, "%ld", seconds);
        assert(n > 0); // Ensure the calculation is successful
        // Allocate memory for the timestamp string
        char* formatted_time = malloc(n + 1);
        if (!formatted_time) {
            return NULL; // Return NULL if memory allocation fails
        }
        // Format the timestamp into the buffer
        snprintf(formatted_time, n + 1, "%ld", seconds);
        return formatted_time;
    }
}

void help(){
  // prints the command line options 
  printf("\nDisplay information for a file.\n\n");
  printf("Command-Line Options\n");
  printf("  -?, --help:                 Display help information about the tool and its options.\n");
  printf("   Example: inspect -?\n");
  printf("  -i, --inode <file_path>:    Display detailed inode information for the specified file.\n");
  printf("   Example: inspect -i /path/to/file\n");
  printf("   Note that this flag is optional and the default behavior is\n");
  printf("   identical if the flag is omitted.\n");
  printf("  -a, --all [directory_path]: Display inode information for all files within the specified directory. If nopaths is provided, default to the current directory.\n");
  printf("   Optional flag: -r, --recursive for recursive listing.\n");
  printf("   Example: inspect -a /path/to/directory -r\n");
  printf("  -h, --human:                Output all sizes in kilobytes (K), megabytes (M), or gigabytes(G) and all dates in a human-readable form.\n");
  printf("   Example: inspect -i /path/to/file -h\n");
  printf("  -f, --format [text|json]:   Specify the output format. If not specified, default to plain text.\n");
  printf("   Example: inspect -i /path/to/file -f json\n");
  printf("  -l, --log <log_file>:       Log operations to a specified file.\n");
  printf("   Example: inspect -i /path/to/file -l /path/to/logfile\n\n");
}



  