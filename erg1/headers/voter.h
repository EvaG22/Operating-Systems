#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/vlist.h"
#include "../headers/vhashtable.h"

typedef struct Voter Voter;
typedef struct Hashtable Hashtable;
typedef struct List List;

size_t totalBytesFreed;

//parses command line arguments
void handleOptions(int argc, char* argv[], char**, int*);

void printVoter(const struct Voter* );

//function for exit command
void peace_out(struct List* , struct Hashtable*);