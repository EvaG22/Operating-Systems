#include "../headers/linked_list.h"
#include "../headers/cmp_functions.h"

void insertEntry(LinkedList *list, const char *name, const char *path, struct stat fileStat, const char *dirName) {
    EntryNode *newNode = (EntryNode *)malloc(sizeof(EntryNode));
    if (newNode == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    // Remove dirName from the beginning of the path so we can make it relative
    const char *cleanedPath = path + strlen(dirName) + 1;

    // Ensure the cleaned path does not exceed PATH_MAX
    if (strlen(cleanedPath) >= PATH_MAX) {
        fprintf(stderr, "Cleaned path exceeds PATH_MAX\n");
        exit(EXIT_FAILURE);
    }

    strcpy(newNode->name, name);
    strcpy(newNode->path, path);  // Update the path without dirName and leading '/'
    strcpy(newNode->relativePath, cleanedPath);
    newNode->fileStat = fileStat;
    newNode->next = NULL;

    // Update the linked list
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

void freeLinkedList(LinkedList List) { // Free the linked list
    EntryNode *temp = List.head;
    while (temp != NULL) {
        EntryNode *next = temp->next;
        free(temp);
        temp = next;
    }
    List.head = NULL;
    List.tail = NULL;
}

void processDirectory(const char *directoryPath, LinkedList *list, const char *dirName) {
    // Open the directory
    DIR *dir = opendir(directoryPath);

    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Ignore special entries "." and ".."
        if (entry->d_name[0] == '.')
            continue;

        // Construct the full path to the entry
        char fullPath[PATH_MAX];
        snprintf(fullPath, PATH_MAX, "%s/%s", directoryPath, entry->d_name);

        struct stat fileStat;
        if (lstat(fullPath, &fileStat) == -1) {
            perror("Error getting file status");
            continue;  
        }

        
        insertEntry(list, entry->d_name, fullPath, fileStat, dirName); // Add the entry to the linked list

        
        if (S_ISDIR(fileStat.st_mode)) {        // If it's a directory, recursively process it
            processDirectory(fullPath, list, dirName);
        }
    }
    closedir(dir);
}

void removeDirectory(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Ignore   "." and ".." 
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullPath[PATH_MAX];
        snprintf(fullPath, PATH_MAX, "%s/%s", path, entry->d_name);

        struct stat fileStat;
        if (lstat(fullPath, &fileStat) == -1) {
            perror("Error getting file status");
            continue;
        }

        if (S_ISDIR(fileStat.st_mode)) { //We use recursively removeDirectory to remove all the files in the directory  
            removeDirectory(fullPath);
        } else {
            if (remove(fullPath) == -1) {
                perror("Error removing file");
            }
        }
    }

    closedir(dir);

    if (rmdir(path) == -1) {
        perror("Error removing directory");
    } 
}