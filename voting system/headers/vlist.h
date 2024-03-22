#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/vhashtable.h"

typedef struct Voter Voter;

typedef struct List_node {
    struct Voter data;
    struct List_node* prev;
    struct List_node* next;
} List_node;

typedef struct List {
    struct List_node* head;
    struct List_node* tail;
} List;

void insertInList(struct List*, struct Voter);
void printList(struct List*);
void printVoter(const struct Voter*);

void m_markVotedList(struct List*, int);
void z_printVotersForZip(struct List*, const char*);

struct ZipCodeList {
    char zipCode[10];
    int count;
    struct ZipCodeList* next;
};
void add(struct ZipCodeList**, const char*);
void sort(struct ZipCodeList**);
void printZipCodeList(struct ZipCodeList*);
void free_list(struct ZipCodeList*);
void o_countVotersPerZip(struct List*);