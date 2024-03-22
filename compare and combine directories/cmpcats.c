#include "headers/cmp_functions.h"
#include "headers/create_new.h"
#include "headers/linked_list.h"

int main(int argc, char *argv[]) {
    
    if ((argc != 4 && argc != 6) || strcmp(argv[1], "-d") != 0) {
        fprintf(stderr, "Usage: %s -d <dirPathA> <dirPathB> [-s <outputDir>]\n", argv[0]);
        return 1;
    }

    // Assign directory paths from command-line arguments
    char *dirPathA = argv[2];
    char *dirPathB = argv[3];

    // Check if directories exist
    struct stat statDirA, statDirB;
    if (stat(dirPathA, &statDirA) == -1 || stat(dirPathB, &statDirB) == -1) {
        perror("Error getting directory status");
        return 1;
    }

    // Check if the paths are directories
    if (!S_ISDIR(statDirA.st_mode) || !S_ISDIR(statDirB.st_mode)) {
        printf("Error: %s or %s is not a directory\n", dirPathA, dirPathB);
        return 1;
    }

    // Initialize linked lists for each directory
    LinkedList entriesA = {NULL, NULL};
    LinkedList entriesB = {NULL, NULL};
    LinkedList foundInA = {NULL, NULL};
    LinkedList foundInB = {NULL, NULL};
    LinkedList foundInBoth = {NULL, NULL};

    // Process the entries in each directory
    processDirectory(dirPathA, &entriesA, dirPathA);
    processDirectory(dirPathB, &entriesB, dirPathB);

    // Compare entries and find missing or additional files
    EntryNode *tempA = entriesA.head;
    while (tempA != NULL) {
        int found = 0;
        EntryNode *tempB = entriesB.head;
        while (tempB != NULL) {
            if (compareFiles(tempA, tempB)) {
                found = 1;
                insertEntry(&foundInBoth, tempB->name, tempB->path, tempB->fileStat, dirPathB);
                break;
            }
            tempB = tempB->next;
        }
        if (!found) {
            insertEntry(&foundInA, tempA->name, tempA->path, tempA->fileStat, dirPathA);
        }
        tempA = tempA->next;
    }

    EntryNode *tempB = entriesB.head;
    while (tempB != NULL) {
        int found = 0;
        EntryNode *tempA = entriesA.head;
        while (tempA != NULL) {
            if (compareFiles(tempB, tempA)) {
                found = 1;
                
                break;
            }
            tempA = tempA->next;
        }
        if (!found) {
            insertEntry(&foundInB, tempB->name, tempB->path, tempB->fileStat, dirPathB);
        }
        tempB = tempB->next;
    }
    
    if (argc == 4) {
        // Print entries found in A, found in B, and found in both
        EntryNode *temp = foundInA.head;
        printf("Entries in %s but not in %s:\n", dirPathA, dirPathB);
        while (temp != NULL) {
            printf("%s\n", temp->relativePath);
            temp = temp->next;
        }

        temp = foundInB.head;
        printf("\nEntries in %s but not in %s:\n", dirPathB, dirPathA);
        while (temp != NULL) {
            printf("%s\n", temp->relativePath);
            temp = temp->next;
        }
        printf("\n");

        temp = foundInBoth.head;
        printf("Entries in both %s and %s:\n", dirPathA, dirPathB);
        while (temp != NULL) {
            printf("%s\n", temp->relativePath);
            temp = temp->next;
        }
    }

    if (argc == 6 && strcmp(argv[4], "-s") == 0) {
        LinkedList mergedDir = {NULL, NULL}; //It will contain solely files with the old inodes in order to find the hard links 

        char* destinationDir = argv[5];

        // Check if the destination directory exists
        struct stat dirStat;
        if (stat(destinationDir, &dirStat) != -1) {
            removeDirectory(destinationDir);
        }

        // Copy files found in both directories to a new directory
        copyFilesToDirectory(&foundInBoth,&foundInA,&foundInB, destinationDir, &mergedDir, &entriesA, &entriesB);
        printf("\n");
        printf("Check the directory %s for the final merged directory\n", destinationDir);
        freeLinkedList(mergedDir);
    }

    // Free the linked lists
    freeLinkedList(entriesA);
    freeLinkedList(entriesB);
    freeLinkedList(foundInA);
    freeLinkedList(foundInB);
    freeLinkedList(foundInBoth);
    
    return 0;
}
