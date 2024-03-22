#include "../headers/bubblesort.h"

void bubblesort(Record records[], int numRecords) {
    for (int i = 0; i < numRecords - 1; i++) {
        for (int j = 0; j < numRecords - i - 1; j++) {
            if (compareRecords(&records[j], &records[j + 1]) > 0) {
                swap(&records[j], &records[j + 1]);
            }
        }
    }
}

int main(int argc, char *argv[]) {

    char *inputFileName = argv[1];
    int inputFilePtr = open(inputFileName, O_RDONLY);
    if (inputFilePtr == -1) {
        perror("open failed");
        return 1;
    }
    
    //moving the file pointer to the correct position for this sorter
    lseek(inputFilePtr, atoi(argv[2]) * sizeof(Record), SEEK_SET);
    
    int size = atoi(argv[3]);
    Record *records = (Record *)malloc(size * sizeof(Record));
    if (records == NULL) {
        perror("memory allocation failed");
        return -1;
    }
    for (int i = 0; i < size; i++) {
        read(inputFilePtr, &records[i], sizeof(Record));
    }

    close(inputFilePtr);

    bubblesort(records, size);
    // printRecords(records, size);

    //open the pipe and write in it the sorted result
    int fd = open(argv[4], O_WRONLY);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }
    if (write(fd, records, size * sizeof(Record)) == -1) {
        perror("write failed");
        close(fd);
        return 1;
    }
    close(fd);

    free(records);

    return 0;
}