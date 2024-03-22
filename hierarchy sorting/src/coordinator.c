#include "../headers/record.h"
#include "../headers/mfunctions.h"

int mergersSignals = 0;
void signal_handler1(int signum) {
    mergersSignals++;
}

int main(int argc, char *argv[]) {

    signal(SIGUSR1, signal_handler1);
    
    if (argc != 9) {
        fprintf(stderr, "Usage: %s -i DataFile -k NumofChildren -e1 sorting1 -e2 sorting2\n", argv[0]);
        return 1;
    }

    char *inputFile = NULL;
    int numOfChildren = 0;
    char *sortPr1 = NULL;
    char *sortPr2 = NULL;
    handleOptions(argc, argv, &inputFile, &numOfChildren, &sortPr1, &sortPr2);

    //opening file to count the lines (records)
    int inputFilePtr = open(inputFile, O_RDONLY);
    if (inputFilePtr == -1) {
        perror("open failed");
        return 1;
    }
    int lineCount = 0;
    Record record;
    while (read(inputFilePtr, &record, sizeof(Record)) > 0) {
        lineCount++;
    }
    close(inputFilePtr);
    int linesPerMerger = lineCount/numOfChildren;

    //will save here the final sorted result
    Record* mergedRecords = (Record*)malloc(lineCount * sizeof(Record));
    int mergedRecordsSize = 0;
    
    char fifo[numOfChildren][8];

    for (int i=0; i<numOfChildren; i++) {

        //creating named pipe for each merger
        sprintf(fifo[i],"merger%d", i);
        if (mkfifo(fifo[i],0666) < 0) {
            perror("fifo creation error");
            return 1;
        } 

        pid_t merger_pid = fork();
        if (merger_pid == -1) {
            perror("fork failed");
            return 1;
        } else if (merger_pid == 0) {

            char linesPerMergerStr[20];
            //the first merger will take care of the case that lines%k != 0
            if (i == 0){
                linesPerMerger += lineCount%numOfChildren;
            }
            sprintf(linesPerMergerStr, "%d", linesPerMerger);

            char numOfChildrenStr[20];
            sprintf(numOfChildrenStr, "%d", numOfChildren);

            char iStr[5];
            sprintf(iStr, "%d", i);
            
            execl("./mergers", "mergers", inputFile, linesPerMergerStr, numOfChildrenStr, iStr, fifo[i], sortPr1, sortPr2, NULL);
        
        } else {

            int fd = open(fifo[i], O_RDONLY);
            if (fd == -1) {
                perror("open failed");
                return 1;
            }

            if (i == 0) { 
                linesPerMerger += atoi(argv[2]) % (numOfChildren-i);
            }
            //temp array to read array with records from pipe
            Record* mergerArray = (Record*)malloc(linesPerMerger * sizeof(Record));
            read(fd, mergerArray, linesPerMerger * sizeof(Record));

            //also reading the time
            double sortTime;
            for (int j=0; j<numOfChildren-i; j++){
                read(fd, &sortTime, sizeof(double));
                printf("Time required from sorter %d of merger %d: %.2lf seconds\n", j, i, sortTime);
            }

            close(fd);

            merge(mergedRecords, mergerArray, mergedRecordsSize, linesPerMerger);
            mergedRecordsSize+=linesPerMerger;
            mergersSignals++;
            check_merged(mergedRecords, mergedRecordsSize);
            free(mergerArray);
            wait(NULL);
        }

    }

    printRecords(mergedRecords, lineCount);
    free(mergedRecords);

    printf("Total SIGUSR1 signals received:%d\n", --mergersSignals);

    return 0;
}