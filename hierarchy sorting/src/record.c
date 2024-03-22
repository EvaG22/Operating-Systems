#include "../headers/record.h"

void printRecords(Record *arr, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i].PIN != 0)
        printf("Last Name: %s, First Name: %s, PIN: %d, Postal Code: %s\n",
        arr[i].lastName, arr[i].firstName, arr[i].PIN, arr[i].postalCode);
    }
}

int compareRecords(Record *record1, Record *record2) {
    int lastNames = strcmp(record1->lastName, record2->lastName);
    if (lastNames != 0) {
        return lastNames;
    }

    int firstNames = strcmp(record1->firstName, record2->firstName);
    if (firstNames != 0) {
        return firstNames;
    }

    return record1->PIN - record2->PIN;
}

void swap(Record *record1, Record *record2) {
    Record temp = *record1;
    *record1 = *record2;
    *record2 = temp;
}

void copyRecord(Record *record1, Record *record2) {
    record1->PIN = record2->PIN;
    strcpy(record1->lastName, record2->lastName);
    strcpy(record1->firstName, record2->firstName);
    strcpy(record1->postalCode, record2->postalCode);
}

void check_merged(Record *records, int numRecords) {
    for (int i = 0; i < numRecords-1; i++) {
        for (int j=0; j < numRecords-i-1; j++) {
            if (compareRecords(&records[j], &records[j+1]) > 0) {
                swap(&records[j], &records[j + 1]);
            }
        }
    }
    for (int i=0; i < numRecords-1; i++) {
        if (compareRecords(&records[i], &records[i+1]) == 0) {
            records[i].PIN=0;
        }
    }
}

void merge(Record *mergedRecords, Record *mergerArray, int size1, int size2) {
    int i = 0, j = 0, k = 0;

    //check if mergedRecords is empty
    if (size1 == 0) {
        while (i < size2) {
            mergedRecords[k++] = mergerArray[i++];
        }
        return;
    }

    //until one of them is empty
    while (i < size1 && j < size2) {
        if (compareRecords(&mergedRecords[i], &mergerArray[j]) <= 0) {
            copyRecord(&mergedRecords[k++], &mergerArray[i++]);
        } else {
            copyRecord(&mergedRecords[k++], &mergerArray[j++]);
        }
    }

    //copy the remaining elements of the array that isn't empty yet
    while (i < size1) {
        copyRecord(&mergedRecords[k++], &mergerArray[i++]);
    }
    while (j < size2) {
        copyRecord(&mergedRecords[k++], &mergerArray[j++]);
    }
}