#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

// Structure to store information about an entry
typedef struct EntryNode {
    char name[PATH_MAX];
    char path[PATH_MAX];
    char relativePath[PATH_MAX];
    struct stat fileStat;
    struct EntryNode *next;
} EntryNode;

// Structure to manage a linked list of entries
typedef struct LinkedList {
    EntryNode *head;
    EntryNode *tail;
} LinkedList;

void insertEntry(LinkedList*, const char*, const char*, struct stat, const char*);
void freeLinkedList(LinkedList);

void processDirectory(const char *, LinkedList *, const char*);
void removeDirectory(const char *);

#endif