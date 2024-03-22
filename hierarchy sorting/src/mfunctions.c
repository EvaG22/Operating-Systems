#include "../headers/record.h"

void handleOptions(int argc, char *argv[], char **inputFile, int *numChildren, char **sortPr1, char **sortPr2) {

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 < argc) {
                *inputFile = argv[i + 1];
                i++;
            } else {
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "-k") == 0) {
            if (i + 1 < argc) {
                *numChildren = atoi(argv[i + 1]);
                i++;
            } else {
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "-e1") == 0) {
            if (i + 1 < argc) {
                *sortPr1 = argv[i + 1];
                i++;
            } else {
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "-e2") == 0) {
            if (i + 1 < argc) {
                *sortPr2 = argv[i + 1];
                i++;
            } else {
                exit(EXIT_FAILURE);
            }
        }
    }   
}