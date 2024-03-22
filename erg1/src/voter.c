#include "../headers/voter.h"
#include "../headers/vhashtable.h"
#include "../headers/vlist.h"

void handleOptions(int argc, char* argv[], char** inputFileName, int* maxBucketEntries) {

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            *inputFileName = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            *maxBucketEntries = atoi(argv[i + 1]);
            i++;
        }
    }
}

void printVoter(const struct Voter* voter) {
    printf("PIN: %d\n", voter->PIN);
    printf("Last Name: %s\n", voter->lastName);
    printf("First Name: %s\n", voter->firstName);
    printf("Postal Code: %s\n", voter->postalCode);
    printf("Has Voted: %s\n\n", voter->hasVoted ? "Yes" : "No");
}

void peace_out(struct List* list, struct Hashtable* ht) {

    while (list->head != NULL) {
        struct List_node* temp = list->head;
        list->head = list->head->next;
        totalBytesFreed += sizeof(struct List_node) + sizeof(struct Voter);
        free(temp);
    }

    for (int i = 0; i < ht->size; i++) {
        struct Bucket* currentBucket = &ht->table[i];
        struct Node* currentNode = currentBucket->head;
        while (currentNode != NULL) {
            struct Node* tempNode = currentNode;
            currentNode = currentNode->next;
            totalBytesFreed += sizeof(struct Node) + sizeof(struct Voter);
            free(tempNode);
        }
    }

    printf("%zu Bytes Released\n", totalBytesFreed);
    exit(0);
}