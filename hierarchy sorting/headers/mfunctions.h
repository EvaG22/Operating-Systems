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
#include "../headers/record.h"

//function to parse command line arguments
void handleOptions(int argc, char *argv[], char**, int*, char**, char**);