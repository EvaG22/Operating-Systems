#ifndef CMP_FUNCTIONS_H
#define CMP_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "../headers/linked_list.h"


int compareDirectories(const EntryNode *, const EntryNode *);
int compareRecords(const EntryNode *, const EntryNode *);
int compareLinks(const EntryNode *, const EntryNode *);
int compareFiles(const EntryNode *, const EntryNode *);

#endif