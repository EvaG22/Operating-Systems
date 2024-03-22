#include "../headers/voter.h"
#include "../headers/vhashtable.h"
#include "../headers/vlist.h"

//sorted based on the postal code
void insertInList(struct List* list, struct Voter voter) {
    
    //creating new node for the voter
    struct List_node* newNode = (struct List_node*)malloc(sizeof(struct List_node));
    newNode->data = voter;
    newNode->prev = NULL;
    newNode->next = NULL;

    //when the list is empty, new node = head
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
        return;
    }

    struct List_node* current = list->head;
    while (current != NULL) {
        if (strcmp(voter.postalCode, current->data.postalCode) <= 0) {
            //insert before current node
            newNode->next = current;
            newNode->prev = current->prev;
            //insert at the beginning of the list
            if (current->prev != NULL) {
                current->prev->next = newNode;
            } else {
                list->head = newNode;
            }
            current->prev = newNode;
            return;
        }
        current = current->next;
    }

    //if not inserted, append to the end
    newNode->prev = list->tail;
    list->tail->next = newNode;
    list->tail = newNode;
}

void printList(struct List* list) {
    struct List_node* current = list->head;
    while (current != NULL) {
        printVoter(&(current->data));
        current = current->next;
    }
}

void m_markVotedList(struct List* list, int pin){
    struct List_node* current = list->head;
    while (current != NULL) {
        if (current->data.PIN == pin){
            current->data.hasVoted = true;
            return;
        }
        current = current->next;
    }
}

void z_printVotersForZip(struct List* list, const char* targetZipCode) {

    int voterCount = 0;
    struct List_node* current = list->head;

    //list with PINs of the voters
    struct PinList {
        int pin;
        struct PinList* next;
    };
    struct PinList* pinsList = NULL;

    while (current != NULL) {
        if (strcmp(current->data.postalCode, targetZipCode) == 0 && current->data.hasVoted) {
            voterCount++;
            struct PinList* newPinNode = (struct PinList*)malloc(sizeof(struct PinList));
            newPinNode->pin = current->data.PIN;
            newPinNode->next = pinsList;
            pinsList = newPinNode;
        }
        current = current->next;
    }

    printf("%d voted in %s\n", voterCount, targetZipCode);
    //printing the PINs
    struct PinList* pinNode = pinsList;
    while (pinNode != NULL) {
        printf("%d\n", pinNode->pin);
        pinNode = pinNode->next;
    }

    //free the structures used
    struct PinList* node = pinsList;
    while (node != NULL) {
        struct PinList* nextPinNode = node->next;
        totalBytesFreed += sizeof(struct PinList);
        free(node);
        node = nextPinNode;
    }

}

void add(struct ZipCodeList** head, const char* zipCode) {
    if(*head != NULL){
        struct ZipCodeList* current = *head;
        while (current != NULL) {
            if (strcmp(current->zipCode, zipCode) == 0) {
                current->count++;
                return;
            }
            current = current->next;
        }
    }
    //zip doesn't exist, add new node
    struct ZipCodeList* newNode = (struct ZipCodeList*)malloc(sizeof(struct ZipCodeList));
    strcpy((char*)newNode->zipCode, zipCode);
    newNode->count = 1;
    newNode->next = *head;
    *head = newNode;
}

void sort(struct ZipCodeList** head) {
    struct ZipCodeList* sortedList = NULL;
    while (*head != NULL) {
        struct ZipCodeList* current = *head;
        *head = current->next;
        if (sortedList == NULL || current->count >= sortedList->count) {
            current->next = sortedList;
            sortedList = current;
        } else {
            struct ZipCodeList* temp = sortedList;
            while (temp->next != NULL && current->count < temp->next->count) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
    }
    *head = sortedList;

}

void printZipCodeList(struct ZipCodeList* head){
    struct ZipCodeList* current = head;
    while (current != NULL) {
        printf("%s %d \n",current->zipCode, current->count);
        current = current->next;
    }
}

void free_list(struct ZipCodeList* head) {
    while (head != NULL) {
        struct ZipCodeList* temp = head;
        head = head->next;
        totalBytesFreed += sizeof(struct ZipCodeList);
        free(temp);
    }
}

void o_countVotersPerZip(struct List* list) {
    struct ZipCodeList* zip_code_list = NULL;
    struct List_node* current = list->head;
    while (current != NULL) {
        if (current->data.hasVoted) {
            add(&zip_code_list, current->data.postalCode);
        }
        current = current->next;
    }
    sort(&zip_code_list);
    printZipCodeList(zip_code_list);
    free_list(zip_code_list);
}