#include "../headers/quicksort.h"

int partition(Record *records, int low, int high) {
    Record pivot = records[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (compareRecords(&records[j], &pivot) < 0) {
            i++;
            swap(&records[i], &records[j]);
        }
    }

    swap(&records[i + 1], &records[high]);
    return i + 1;
}

void quicksort(Record *records, int low, int high) {
    if (low < high) {
        int pi = partition(records, low, high);
        quicksort(records, low, pi - 1);
        quicksort(records, pi + 1, high);
    }
}

int main(int argc, char *argv[]) {

    char *inputFile = argv[1];
    int inputFilePtr = open(inputFile, O_RDONLY);
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

    quicksort(records, 0, size - 1);
    //printRecords(records, size);
    
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