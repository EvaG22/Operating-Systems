#ifndef PROCEDURES_H
#define PROCEDURES_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_PROCESS_IDS 100

time_t program_start_time;

typedef struct {
    int customer_id;
    char name[20];
    char lastname[20];
    int balance;
} Record;
Record* records;
int record_count;

// The turnstile semaphore controls entry to the critical section for both readers and writers.
// The mutex semaphore protects the readers_count variable.
// The write_mutex semaphore ensures exclusive access for writers.
typedef struct {
    int readers_count;
    int writers_count;
    sem_t turnstile;
    sem_t mutex;
    sem_t write_mutex;  // Mutex for write access
    pthread_t active_readers[MAX_PROCESS_IDS];
    pthread_t active_writers[MAX_PROCESS_IDS];
    char *shmem;
} SharedMemory;
SharedMemory* shared_memory;

typedef struct {
    int num_readers;
    double total_reader_time;
    int num_writers;
    double total_writer_time;
    time_t max_delay_start;
    int total_records_processed;
} Statistics;
Statistics* statistics;

void initialize_statistics();
void initialize_shared_memory();

int count_records(FILE*);
int load_records_from_file(const char*);
void print_records(int);

void update_statistics_start_time();
void print_statistics();

void cleanup();

#endif
