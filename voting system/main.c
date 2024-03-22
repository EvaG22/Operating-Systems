#include "headers/vlist.h"
#include "headers/vhashtable.h"
#include "headers/voter.h"

int main(int argc, char *argv[]) {

    totalBytesFreed = 0;
    char* inputFileName = NULL;
    int maxBucketEntries = 0;
    handleOptions(argc, argv, &inputFileName, &maxBucketEntries);

    FILE *registeredvoters = fopen(inputFileName, "r");
    if (registeredvoters == NULL) {
        printf("Unable to open file %s\n", inputFileName);
        return 2;
    }

    if (maxBucketEntries == 0) {
        printf("Max number of keyes per bucket is not defined\n");
        return 1;
    }

    Hashtable ht = createHashTable(TABLE_SIZE, 0, 0, maxBucketEntries);

    char line[100];
    while (fgets(line, sizeof(line), registeredvoters) != NULL) {
        struct Voter* voter = (struct Voter*)malloc(sizeof(struct Voter));
        if (sscanf(line, "%d %s %s %s", &voter->PIN, voter->lastName, voter->firstName, voter->postalCode) == 4) {
            voter->hasVoted = false;
            addVoter(&ht, voter);
        } else {
            printf("Error reading line: %s\n", line);
            totalBytesFreed += sizeof(struct Voter);
            free(voter);
        }
    }

    fclose(registeredvoters);

    //initializing the list
    struct List Voters;
    Voters.head = NULL;
    Voters.tail = NULL;

    //scaning Hashtable to add to the list
    for (int i = 0; i < ht.size; i++) {
        struct Bucket bucket = ht.table[i];
        struct Node* current = bucket.head;

        while (current != NULL) {
            insertInList(&Voters, current->data);         
            current = current->next;
        }
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        struct Bucket* bucket = &ht.table[i];
        struct Node* current = bucket->head;

        while (current != NULL) {
            insertInList(&Voters, current->data);
            current = current->next;
        }
    }

    //printHashtable(&ht);
    // printf("\n\n\n\n...............\n\n\n\n\n");
    //printList(&Voters);

    char command[6];
    while (1) {
        printf("Insert the command to be executed: ");
        scanf("%s", command);

        if (strcmp(command, "l") == 0) {
            int pin;
            printf("Enter the PIN: ");
            if (scanf("%d", &pin) == 1) {
                printf("executing command l...\n");
                l_findPin(&ht, pin);
            } else {
                printf("Malformed Pin\n\n");
                continue;
            }
    
        } else if (strcmp(command, "i") == 0) {
            int pin;
            bool success = false;
            printf("Enter the PIN: ");
            if (scanf("%d", &pin) == 1) {
                char lastName[50];
                printf("Enter the last name: ");
                if (scanf("%s", lastName) == 1){
                    char firstName[50];
                    printf("Enter the first name: ");
                    if (scanf("%s", firstName) == 1){
                        char postalCode[10];
                        printf("Enter the postal code: ");
                        if (scanf("%s", postalCode) == 1){
                            success = true;
                            //add to the hash_table
                            i_addVoter(&ht, pin, lastName, firstName, postalCode);
                            //create the new voter to insert in list
                            struct Voter* newVoter = (struct Voter*)malloc(sizeof(struct Voter));
                            newVoter->PIN = pin;
                            strcpy(newVoter->lastName, lastName);
                            strcpy(newVoter->firstName, firstName);
                            strcpy(newVoter->postalCode, postalCode);
                            newVoter->hasVoted = false;
                            insertInList(&Voters, *newVoter);
                            totalBytesFreed += sizeof(struct Voter);
                            free(newVoter);
                        }
                    }
                }
            }
            if (!success){
                printf("Malformed Input\n\n");
                continue;
            }
            
        } else if (strcmp(command, "m") == 0) {
            int pin;
            printf("Enter the PIN: ");
            if (scanf("%d", &pin) != 1) {
                printf("Malformed Input\n\n");
            } else {
                printf("executing command m...\n");
                m_markVoted(&ht, pin);
                m_markVotedList(&Voters, pin);
            }
            
        } else if (strcmp(command, "bv") == 0) {
            char filename[100];
            printf("Enter the filename: ");
            scanf("%s", filename);
            printf("executing command bv...\n");
            bv_allVoted(&ht, &Voters, filename);

        } else if (strcmp(command, "v") == 0) {
            printf("executing command v...\n");
            int votedCount = v_countVoters(&ht);
            printf("Voted So Far %d\n\n", votedCount);

        } else if (strcmp(command, "perc") == 0) {
            double percentage = perc(&ht);
            printf("executing command perc...\n");
            printf("%.2lf%%\n\n", percentage);

        } else if (strcmp(command, "z") == 0) {
            char postalCode[10];
            printf("Enter the postal code: ");
            scanf("%s", postalCode);
            printf("executing command z...\n");
            z_printVotersForZip(&Voters, postalCode);
            
        } else if (strcmp(command, "o") == 0) {
            printf("executing command o...\n");
            o_countVotersPerZip(&Voters);
            
        } else if (strcmp(command, "exit") == 0) {
            printf("Exiting...\n");
            peace_out(&Voters, &ht);
            break;

        } else {
            printf("Non valid command. Insert again:\n");
        }

    }

    return 0;
}