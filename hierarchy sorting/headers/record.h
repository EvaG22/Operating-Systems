#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

typedef struct Record {
    int PIN;
    char lastName[20];
    char firstName[20];
    char postalCode[6];
} Record;

//functions used for the sorting methods
int compareRecords(Record*, Record*);
void swap(Record*, Record*);

//function used for the merging
void copyRecord(Record*, Record*);
void check_merged(Record*, int);
void merge(Record*, Record*, int, int);

void printRecords(Record*, int);

int kill(pid_t pid, int sig);