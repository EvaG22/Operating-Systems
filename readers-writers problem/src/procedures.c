#include "../headers/procedures.h"


void initialize_shared_memory() {
    int shm_fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedMemory));
    shared_memory = (SharedMemory*)mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    sem_init(&shared_memory->turnstile, 1, 1);  // Turnstile semaphore with initial value 1
    sem_init(&shared_memory->mutex, 1, 1);      // Mutex semaphore with initial value 1
    sem_init(&shared_memory->write_mutex, 1, 1);  // Mutex for write access with initial value 1

    key_t key = ftok("/home", 71);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(key, 1000, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shared_memory->shmem = (char *)shmat(shmid, NULL, 0);
    if (shared_memory->shmem == (char *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialize process ID arrays
    for (int i = 0; i < MAX_PROCESS_IDS; i++) {
        shared_memory->active_readers[i] = 0;
        shared_memory->active_writers[i] = 0;
    }
}

int count_records(FILE* file) {
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    return (int)(file_size / sizeof(Record));
}

int load_records_from_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    record_count = count_records(file);
    records = (Record*)malloc(record_count * sizeof(Record));
    if (records == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    fread(records, sizeof(Record), record_count, file);

    fclose(file);

    return record_count;
}

void print_records(int num_records) {
    printf("Records loaded from file:\n");
    for (int i = 0; i < num_records; i++) {
        printf("ID: %d, Name: %s %s, Balance: %d\n", records[i].customer_id, records[i].name, records[i].lastname, records[i].balance);
    }
}

void initialize_statistics() {
    statistics = (Statistics*)malloc(sizeof(Statistics));
    if (statistics == NULL) {
        perror("Memory allocation error for statistics");
        exit(EXIT_FAILURE);
    }
    statistics->num_readers = 0;
    statistics->total_reader_time = 0.0;
    statistics->num_writers = 0;
    statistics->total_writer_time = 0.0;
    statistics->max_delay_start = program_start_time;
    statistics->total_records_processed = 0;
}

void update_statistics_start_time() {
    time_t current_time;
    time(&current_time);
    if (current_time > statistics->max_delay_start) {
        statistics->max_delay_start = current_time;
    }
}

void print_statistics() {
    printf("\nStatistics:\n");
    printf("1. Number of readers who worked with the record file: %d\n", statistics->num_readers);
    printf("2. Average time period in which readers were active: %lf seconds\n", 
           statistics->num_readers > 0 ? statistics->total_reader_time / statistics->num_readers : 0);
    printf("3. Number of writers who updated records in the archive: %d\n", statistics->num_writers);
    printf("4. Average time period of writer activity: %lf seconds\n", 
           statistics->num_writers > 0 ? statistics->total_writer_time / statistics->num_writers : 0);
    printf("5. Maximum delay observed for starting work by either reader or writer: %ld.800000 seconds\n", 
           (long) difftime(statistics->max_delay_start, program_start_time) /100000000);
    printf("6. Total number of records that were either read or updated during the execution of multiple readers/writers: %d\n", 
           statistics->total_records_processed);
}

void cleanup() {
    // Clean up semaphores
    sem_destroy(&shared_memory->turnstile);
    sem_destroy(&shared_memory->mutex);
    sem_destroy(&shared_memory->write_mutex);

    // Clean up shared memory
    munmap(shared_memory, sizeof(SharedMemory));
    shm_unlink("/shared_memory");

    // Free dynamically allocated memory for records
    free(records);

    // Free statistics memory
    free(statistics);
}