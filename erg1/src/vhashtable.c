#include "../headers/voter.h"
#include "../headers/vhashtable.h"
#include "../headers/vlist.h"

struct Hashtable createHashTable(int size, int total_buckets, int round, int max_bucket_entries){
    Hashtable ht;
    memset(&ht, 0, sizeof(ht)); 
    ht.round = round;
    ht.size = size;
    ht.total_buckets = total_buckets;
    ht.max_bucket_entries = max_bucket_entries;
    ht.p = ht.table[0];

    for (int i = 0; i < size; i++) {
        ht.table[i].count = 0;
        ht.table[i].extras = 0;
    }
    return ht;
}

int p(struct Hashtable* ht) {
    for (int i = 0; i < ht->total_buckets; i++) {
        if (&ht->table[i] == &ht->p) {
            return i;
        }
    }
    return -1;
}

int hash(int PIN, int i, int m, struct Hashtable* ht) {
    int index = PIN % ((1 << i) * m);
    if (index < p(ht)) {
        index = PIN % ((1 << i) * (m+1));
    }
    return index;
}

int allKeys(struct Hashtable* ht) {
    int sum = 0;
    for (int i = 0; i < ht->total_buckets; i++) {
        sum += ht->table[i].count;
        sum += ht->table[i].extras;
    }
    return sum;
}

float loadFactor(struct Hashtable* ht) {
    float lamda = (float)(allKeys(ht)) / (float)(ht->size * ht->total_buckets);
    return lamda;
}

void split(struct Hashtable* ht) {
    int new_size = ht->size * 2;
    ht->total_buckets++;
    struct Hashtable new_ht = createHashTable(new_size, ht->total_buckets, ht->round, ht->max_bucket_entries);
    
    //loop through the existing hashtable and rehash the keys into the new hashtable
    for (int i = 0; i < ht->size; i++) {
        struct Node* current = ht->table[i].head;
        while (current != NULL) {
            //calculate the new index using the updated hashtable size
            int new_index = hash(current->data.PIN, ht->round + 1, new_size, ht);
            struct Node* next = current->next;

            if (new_ht.table[new_index].count < new_size) {
                if (new_ht.table[new_index].head == NULL) {
                    new_ht.table[new_index].head = current;
                } else {
                    current->next = new_ht.table[new_index].head;
                    new_ht.table[new_index].head = current;
                }
                new_ht.table[new_index].count++;
            } else {
                new_ht.table[new_index].extras++;
                if (new_ht.table[new_index].head == NULL) {
                    new_ht.table[new_index].head = current;
                } else {
                    current->next = new_ht.table[new_index].head;
                    new_ht.table[new_index].head = current;
                }
            }

            current = next;
        }
    }

    //update the p pointer
    int index = p(ht);
    ht->p = ht->table[index + 1];

    //free the memory of the old hashtable
    for (int i = 0; i < ht->size; i++) {
        struct Node* current = ht->table[i].head;
        while (current != NULL) {
            struct Node* next = current->next;
            totalBytesFreed += sizeof(struct Node);
            free(current);
            current = next;
        }
    }

    //update the original hashtable to point to the new hashtable
    ht->size = new_size;
    ht->round++;
    *ht = new_ht;
}

void addVoter(struct Hashtable* ht, struct Voter* voter) {

    int index = hash(voter->PIN, ht->round, ht->size, ht);
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = *voter;
    newNode->next = NULL;

    if (ht->table[index].count < ht->size) {
        if (ht->table[index].head == NULL) {
            ht->table[index].head = newNode;
        } else {
            newNode->next = ht->table[index].head;
            ht->table[index].head = newNode;
        }
        ht->table[index].count++;
    } else {
        ht->table[index].extras++;
        if (ht->table[index].head == NULL) {
            ht->table[index].head = newNode;
        } else {
            newNode->next = ht->table[index].head;
            ht->table[index].head = newNode;
        }
    }

    if (loadFactor(ht) > 0.75) {
        split(ht);
    }
    free(voter);
}

void printHashtable(const struct Hashtable* ht) {
    for (int i = 0; i < ht->size; i++) {
        struct Node* current = ht->table[i].head;
        while (current != NULL) {
            printVoter(&(current->data));
            current = current->next;
        }
    }
}

void l_findPin(struct Hashtable* ht, int pin) {
    int index = hash(pin, ht->round, ht->size, ht);

    struct Bucket* bucket = &ht->table[index];
    struct Node* current = bucket->head;

    while (current != NULL) {
        if (current->data.PIN == pin) {
            printVoter(&(current->data));
            return;
        }
        current = current->next;
    }

    printf("Participant %d not in cohort\n\n", pin);
}

void i_addVoter(struct Hashtable* ht, int pin, const char* lastName, const char* firstName, const char* postalCode) {
    
    int index = hash(pin, ht->round, ht->size, ht);
    struct Bucket* bucket = &ht->table[index];
    struct Node* current = bucket->head;

    while (current != NULL) {
        if (current->data.PIN == pin) {
            printf("PIN %d already exists\n\n", pin);
            return;
        }
        current = current->next;
    }

    struct Voter newVoter;
    newVoter.PIN = pin;
    strncpy(newVoter.lastName, lastName, sizeof(newVoter.lastName) - 1);
    newVoter.lastName[sizeof(newVoter.lastName) - 1] = '\0';
    strncpy(newVoter.firstName, firstName, sizeof(newVoter.firstName) - 1);
    newVoter.firstName[sizeof(newVoter.firstName) - 1] = '\0';
    strncpy(newVoter.postalCode, postalCode, sizeof(newVoter.postalCode) - 1);
    newVoter.postalCode[sizeof(newVoter.postalCode) - 1] = '\0';
    newVoter.hasVoted = false;

    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = newVoter;
    newNode->next = NULL;

    if (bucket->head == NULL) {
        bucket->head = newNode;
    } else {
        newNode->next = bucket->head;
        bucket->head = newNode;
    }

    bucket->count++;
    printf("%d %s %s %s", newVoter.PIN, newVoter.lastName, newVoter.firstName, newVoter.postalCode); 
    printf(" %s\n\n", newVoter.hasVoted ? "Yes" : "No");   
    
    if (loadFactor(ht) > 0.75) {
        split(ht);
    }
}

void m_markVoted(struct Hashtable* ht, int pin) {

    int index = hash(pin, ht->round, ht->size, ht);
    struct Bucket* bucket = &ht->table[index];
    struct Node* current = bucket->head;
    bool voterFound = false;

    while (current != NULL) {
        if (current->data.PIN == pin) {
            voterFound = true;
            if (current->data.hasVoted) {
                printf("|%d| is already marked as voted\n", pin);
            } else {
                current->data.hasVoted = true;
                printf("|%d| Marked as voted\n", pin);
            }
            break;
        }
        current = current->next;
    }

    if (!voterFound) {
        printf("|%d| does not exist\n\n", pin);
    }
}

void bv_allVoted(struct Hashtable* ht, struct List* list, const char* filename) {

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n\n", filename);
        return;
    }

    bool found;
    int pin;
    int index;
    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "%d", &pin) != 1) {
            printf("Malformed Input\n\n");
        } else {
            found = false;
            index = hash(pin, ht->round, ht->size, ht);
            struct Bucket* bucket = &ht->table[index];
            struct Node* current = bucket->head;
            while (current != NULL) {
                if (current->data.PIN == pin) {
                    found = true;
                    //also update hasVoted in the list
                    m_markVotedList(list, pin);
                    if (current->data.hasVoted) {
                        printf("|%d| is already marked as voted\n", pin);
                    } else {
                        current->data.hasVoted = true;
                        printf("|%d| Marked as voted\n", pin);
                    }
                    break;
                }
                current = current->next;
            }
        }
        if (found == false) {
            printf("%d does not exist\n\n", pin);
        }
    }

    fclose(file);
}

int v_countVoters(struct Hashtable* ht) {

    int count = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        struct Bucket* bucket = &ht->table[i];
        struct Node* current = bucket->head;
        
        while (current != NULL) {
            if (current->data.hasVoted) {
                count++;
            }
            current = current->next;
        }
    }
    return count;
}

double perc(struct Hashtable* ht) {

    int totalParticipants = 0;
    int votedParticipants = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        struct Bucket* bucket = &ht->table[i];
        struct Node* current = bucket->head;
        
        while (current != NULL) {
            totalParticipants++;
            if (current->data.hasVoted) {
                votedParticipants++;
            }
            current = current->next;
        }
    }
    if (totalParticipants == 0) { 
        return 0.0; 
    }

    return (double)votedParticipants / totalParticipants * 100.0;
}

