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
    char *logPath;
    char *path;
} Options = {0, 0, 0, 0, 0, 1, 0, NULL, NULL}; // Initialize all options to default values


void print_JSON_Output(const char*, const char*, const char*, const char*, const char*, const char*, const char*, const char*, const char*, const char*, const char*);
void help();
int FilePath(char*);
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    struct stat fileInfo;
    if (stat(argv[1], &fileInfo) != 0) {
        fprintf(stderr, "Error getting file info for %s: %s\n", argv[1], strerror(errno));
        return 1; //status code of 1 indicates error 
    }

    printf("Information for %s:\n", argv[1]);
    printf("File Inode: %llu\n", (unsigned long long)fileInfo.st_ino);
    printf("File Type: ");
    if (S_ISREG(fileInfo.st_mode)) printf("regular file\n");
    else if (S_ISDIR(fileInfo.st_mode)) printf("directory\n");
    else if (S_ISCHR(fileInfo.st_mode)) printf("character device\n");
    else if (S_ISBLK(fileInfo.st_mode)) printf("block device\n");
    else if (S_ISFIFO(fileInfo.st_mode)) printf("FIFO (named pipe)\n");
    else if (S_ISLNK(fileInfo.st_mode)) printf("symbolic link\n");
    else if (S_ISSOCK(fileInfo.st_mode)) printf("socket\n");
    else printf("unknown?\n");

    printf("Permissions: %s\n", getPermissions(fileInfo));

    // Correctly calling print_JSON_Output with all required arguments
    print_JSON_Output(argv[1], 
                      getNumber(fileInfo), 
                      getType(fileInfo), 
                      getPermissions(fileInfo), 
                      getLinkCount(fileInfo), 
                      getUid(fileInfo), 
                      getGid(fileInfo), 
                      getSize(fileInfo), 
                      getAccessTime(fileInfo, Options.human), 
                      getModTime(fileInfo, Options.human), 
                      getStatusChangeTime(fileInfo, Options.human));  // Corrected argument here

    return 0; // status code of 0 means success
}


// print detailed info about JSON file 
void print_JSON_Output(const char* path, const char* number, const char* type,
                         const char* permissions, const char* linkCount,
                         const char* uid, const char* gid, const char* size,
                         const char* accessTime, const char* modTime,
                         const char* statusChangeTime) {
    static char str[MAX_STRING]; // Fixed-size character array
    int length = snprintf(str, MAX_STRING,
        "  {\n    \"filepath\": \"%s\",\n    \"inode\": {\n"
        "      \"number\": %s,\n      \"type\": \"%s\",\n"
        "      \"permissions\": \"%s\",\n      \"linkCount\": %s,\n"
        "      \"uid\": %s,\n      \"gid\": %s,\n      \"size\": %s,\n"
        "      \"accessTime\": %s,\n      \"modificationTime\": %s,\n"
        "      \"statusChangeTime\": %s\n    }\n  },\n",
        path, number, type, permissions, linkCount, uid,
        gid, size, accessTime, modTime, statusChangeTime);
    if (length >= MAX_STRING) {
        // Handle overflow error here
        // This implementation does not support handling overflow
    }
    printf("%s", str);
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

char* getNumber(struct stat fileInfo) {
    static char buffer[20];
    snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)fileInfo.st_ino);
    return buffer;
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
    static char buffer[20];
    snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)fileInfo.st_size);
    return buffer;
}

char* getStatusChangeTime(struct stat fileInfo, int human) {
    if (human) {
        struct tm *info = localtime(&fileInfo.st_ctime);
        static char buffer[30];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", info);
        return buffer;
    } else {
        static char buffer[20];
        snprintf(buffer, sizeof(buffer), "%ld", fileInfo.st_ctime);
        return buffer;
    }
}

char* getAccessTime(struct stat fileInfo, int human) {
    if (human) {
        struct tm *info;
        // Convert time_t to broken-down time representation
        info = localtime(&fileInfo.st_atime);
        // Define a buffer large enough for the formatted date string
        char buffer[30];  // "YYYY-MM-DD HH:MM:SS" will fit comfortably
        size_t result = strftime(buffer, sizeof(buffer), "\"%Y-%m-%d %H:%M:%S\"", info);
        // Check if strftime returned 0, which means the buffer was too small or another error occurred
        if (result == 0) {
            return NULL;  // Handle error appropriately, return NULL or handle differently
        }
        // Allocate memory for the formatted string to return
        char* formatted_time = malloc(result + 1);
        if (formatted_time) {
            strcpy(formatted_time, buffer);
        }
        return formatted_time;
    } else {
        time_t seconds = fileInfo.st_atime;
        // Directly create buffer and format the timestamp
        char buffer[21];  // Large enough to hold any 64-bit integer (up to 20 digits) plus null-terminator
        snprintf(buffer, sizeof(buffer), "%ld", seconds);
        // Allocate memory for the buffer and copy the formatted string
        char* formatted_time = malloc(strlen(buffer) + 1);
        if (formatted_time) {
            strcpy(formatted_time, buffer);
        }
        return formatted_time;
    }
}

char* getModTime(struct stat fileInfo, int human) {
    if (human) {
        struct tm *info;
        // Convert time_t to broken-down time representation
        info = localtime(&fileInfo.st_mtime);  // Using st_mtime for modification time
        // Define a buffer large enough for the formatted date string
        char buffer[30];  // "YYYY-MM-DD HH:MM:SS" will fit comfortably
        size_t result = strftime(buffer, sizeof(buffer), "\"%Y-%m-%d %H:%M:%S\"", info);
        // Check if strftime returned 0, which means the buffer was too small or another error occurred
        if (result == 0) {
            return NULL;  // Handle error appropriately, return NULL or handle differently
        }
        // Allocate memory for the formatted string to return
        char* formatted_time = malloc(result + 1);
        if (formatted_time) {
            strcpy(formatted_time, buffer);
        }
        return formatted_time;
    } else {
        time_t seconds = fileInfo.st_mtime;  // Using st_mtime for modification time
        // Directly create buffer and format the timestamp
        char buffer[21];  // Large enough to hold any 64-bit integer (up to 20 digits) plus null-terminator
        snprintf(buffer, sizeof(buffer), "%ld", seconds);
        // Allocate memory for the buffer and copy the formatted string
        char* formatted_time = malloc(strlen(buffer) + 1);
        if (formatted_time) {
            strcpy(formatted_time, buffer);
        }
        return formatted_time;
    }
}