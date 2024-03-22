#ifndef VHASHTABLE_H
#define VHASHTABLE_H
#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/voter.h"
#include "../headers/vlist.h"

#define TABLE_SIZE 2
typedef struct List List;

typedef struct Voter {
    int PIN;
    char lastName[20];
    char firstName[20];
    char postalCode[10];
    bool hasVoted;
} Voter;

typedef struct Node {
    struct Voter data;
    struct Node* next;
} Node;

typedef struct Bucket {
    struct Node* head; 
    int count;
    int extras;
} Bucket;

typedef struct Hashtable {
    struct Bucket table[TABLE_SIZE];
    int round;
    int size;       //size of each bucket  
    int total_buckets;  
    int max_bucket_entries;             
    struct Bucket p;
} Hashtable;

struct Hashtable createHashTable(int, int, int, int);

int p(struct Hashtable*);
int hash(int, int, int, struct Hashtable*);

int allKeys(struct Hashtable*);
float loadFactor(struct Hashtable*);
void split(struct Hashtable*);

void addVoter(struct Hashtable*, struct Voter*);
void printHashtable(const struct Hashtable*);

void l_findPin(struct Hashtable*, int);
void i_addVoter(struct Hashtable*, int, const char*, const char*, const char*);
void m_markVoted(struct Hashtable*, int);
void bv_allVoted(struct Hashtable*, struct List*, const char*);
int v_countVoters(struct Hashtable*);
double perc(struct Hashtable*);

#endif