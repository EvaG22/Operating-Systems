#include "../headers/record.h"
#include <math.h>

int main(int argc, char *argv[]) {

    char* sortPr1 = argv[6];
    char* sortPr2 = argv[7];

    double t1, t2;
    struct tms tb1, tb2;
    double ticspersec;
    ticspersec = (double)sysconf(_SC_CLK_TCK);
    t1 = (double)times(&tb1);

    pid_t sorting_pid = fork();
    if (sorting_pid == -1) {
        perror("fork failed");
        return 1;
    } else if (sorting_pid == 0) {
        char executable[20];
        //1st is the file, 2nd is the line to go, 3rd is the num of lines, 4th is the pipe
        if (atoi(argv[2]) % 2 == 0) {
            snprintf(executable, sizeof(executable), "./%s", sortPr1);
            execl(executable, sortPr1, argv[1], argv[3], argv[4], argv[5], NULL);
        } else { 
            snprintf(executable, sizeof(executable), "./%s", sortPr2);
            execl(executable, sortPr2, argv[1], argv[3], argv[4], argv[5], NULL);        
        }

        //send sorting time
        t2 = (double)times(&tb2);
        double sortTime = (t2 - t1) / ticspersec;
        sortTime = round(sortTime * 1000000) / 1000000; // round to nearest microsecond
        char sTime[sizeof(double)];
        memcpy(sTime, &sortTime, sizeof(double));
        
        //open the pipe and write the sorting time
        int fd = open(argv[5], O_WRONLY);
        if (fd == -1) {
            perror("open failed");
            return 1;
        }

        write(fd, &sTime, sizeof(double));

        close(fd);

    } else {
        wait(NULL);
    }

    //sending SIGUSR2 to the parent
    kill(getppid(), SIGUSR2);

    return 0;
}
