#include "../headers/cmp_functions.h"


int compareDirectories(const EntryNode *dirA, const EntryNode *dirB) {
    return (strcmp(dirA->relativePath, dirB->relativePath) == 0 && strcmp(dirA->name, dirB->name) == 0);
}

int compareRecords(const EntryNode *recordA, const EntryNode *recordB) {   //Records refer to files and hardlinks 
    if (strcmp(recordA->relativePath, recordB->relativePath) != 0 ||       //(cause they're considered regular files from the system)
        strcmp(recordA->name, recordB->name) != 0 ||
        recordA->fileStat.st_size != recordB->fileStat.st_size) {
        return 0;
    }

    int fileA = open(recordA->path, O_RDONLY);
    int fileB = open(recordB->path, O_RDONLY);
    if (fileA == -1 || fileB == -1) {
        perror("Error opening files for comparison");
        return 0;
    }

    char bufferA[1024]; 
    char bufferB[1024];
    ssize_t bytesReadA, bytesReadB;
    while ((bytesReadA = read(fileA, bufferA, sizeof(bufferA))) > 0) {
        bytesReadB = read(fileB, bufferB, bytesReadA);
        if (bytesReadB != bytesReadA || memcmp(bufferA, bufferB, bytesReadA) != 0) {
            close(fileA);
            close(fileB);
            return 0;
        }
    }

    if (bytesReadA < 0 || read(fileB, bufferB, 1) > 0) {
        close(fileA);
        close(fileB);
        return 0;
    }
    close(fileA);
    close(fileB);

    return 1;
}

int compareLinks(const EntryNode *linkA, const EntryNode *linkB) {
    // Check if both entries are symbolic 
    if (strcmp(linkA->relativePath, linkB->relativePath) == 0){
        if (linkA->fileStat.st_nlink != 1 && linkB->fileStat.st_nlink != 1) {
            return  (linkA->fileStat.st_ino == linkB->fileStat.st_ino);
        }else {
            char targetA[PATH_MAX], targetB[PATH_MAX];
            ssize_t lenA = readlink(linkA->path, targetA, sizeof(targetA) - 1);
            ssize_t lenB = readlink(linkB->path, targetB, sizeof(targetB) - 1);

            if (lenA != -1) targetA[lenA] = '\0';
            if (lenB != -1) targetB[lenB] = '\0';

            return 1;
        }
    }else {
        return 0;
    }
}

int compareFiles(const EntryNode *fileA, const EntryNode *fileB) {
    mode_t modeA = fileA->fileStat.st_mode;
    mode_t modeB = fileB->fileStat.st_mode;

    if (S_ISDIR(modeA) && S_ISDIR(modeB))
        return compareDirectories(fileA, fileB);
    else if (S_ISLNK(modeA) && S_ISLNK(modeB))
        return compareLinks(fileA, fileB);
    else if (S_ISREG(modeA) && S_ISREG(modeB))
        return compareRecords(fileA, fileB);
    else
        return 0;
}