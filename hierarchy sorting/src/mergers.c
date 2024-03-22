#include "../headers/record.h"

int sorterSignals = 0;
void signal_handler2(int signum) {
    sorterSignals++;
}

int main(int argc, char *argv[]) {

    signal(SIGUSR2, signal_handler2);

    int numOfChildren = atoi(argv[3]);
    int i = atoi(argv[4]);

    //argv[2] is lines per merger
    int linesPerSorter = atoi(argv[2]) / (numOfChildren - i);

    char fifoo[numOfChildren - i][10];

    for (int j=0; j < numOfChildren-i; j++) {
        //creating named pipe for each sorter
        sprintf(fifoo[j], "sorter%d%d", i, j);
        if (mkfifo(fifoo[j], 0666) < 0) {
            perror("Fifo creation error");
            return 1;
        }

        pid_t sorter_pid = fork();
        if (sorter_pid == -1) {
            perror("fork failed");
            return 1;
        } else if (sorter_pid == 0) {

            //the first sorter will take care of the case that there are extra lines
            if (j == 0) {
                linesPerSorter += atoi(argv[2]) % (numOfChildren-i);
            }
            char goToLineStr[5];
            sprintf(goToLineStr, "%d", (i*atoi(argv[2]) + j*linesPerSorter));

            char numOfLinesStr[5];
            sprintf(numOfLinesStr, "%d", linesPerSorter);

            char jStr[5];
            sprintf(jStr, "%d", j);

            execl("./sorters", "sorters", argv[1], jStr, goToLineStr, numOfLinesStr, fifoo[j], argv[6], argv[7], NULL);
        } 
    }

    //will save here the result of all the sorters
    Record *mergedRecords = (Record *)malloc(atoi(argv[2]) * sizeof(Record));
    int mergedRecordsSize = 0;
    
    for (int j=0; j < numOfChildren-i; j++) {

        int fd = open(fifoo[j], O_RDONLY);
        if (fd == -1) {
            perror("error opening pipe for reading");
            return 1;
        }

        //the first sorter of each merger probably sorted more records
        if (j == 0) { 
            linesPerSorter += atoi(argv[2]) % (numOfChildren-i);
        }
        
        //temp array to read array with records from pipe
        Record *mergerArray = (Record *)malloc(linesPerSorter * sizeof(Record));
        read(fd, mergerArray, linesPerSorter * sizeof(Record));
        
        merge(mergedRecords, mergerArray, mergedRecordsSize, linesPerSorter);
        mergedRecordsSize += linesPerSorter;
        free(mergerArray);

        //also read the time for pipe
        double sortTime;
        read(fd, &sortTime, sizeof(double));

        close(fd);

        //open the merger pipe so as to wite there the results of the sorter pipes
        int merger_fd = open(argv[5], O_WRONLY);
        if (merger_fd == -1) {
            perror("error opening pipe for writing");
            return 1;
        }

        //write the entire mergedRecords array and the time to the output pipe
        write(merger_fd, mergedRecords, mergedRecordsSize * sizeof(Record));
        free(mergedRecords);
        write(merger_fd, &sortTime, sizeof(double));

        close(merger_fd);

        wait(NULL);
    }

    for (int j = 0; j < numOfChildren; j++) {
        printf("Total SIGUSR2 signals for merger %d received:%d\n", j, sorterSignals);
    }

    //sending SIGUSR2 to the parent
    kill(getppid(), SIGUSR1);

    return 0;
}
