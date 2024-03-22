#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../headers/record.h"

int partition(Record*, int, int);
void quicksort(Record*, int, int);