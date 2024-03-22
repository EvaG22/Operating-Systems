#include "../headers/procedures.h"
#include "../headers/read_write_proc.h"


void* reader(void* arg) {
    
    time_t start_time;
    time(&start_time);
    pid_t pid = getpid();
    update_statistics_start_time();
    
    sem_wait(&shared_memory->turnstile);  // Wait in line
    sem_post(&shared_memory->turnstile);  // Allow the next in line
    sem_wait(&shared_memory->mutex);  // Acquire mutex to protect the shared resource
    shared_memory->readers_count++;
    if (shared_memory->readers_count == 1) {
        sem_wait(&shared_memory->write_mutex);  // If the first reader, block writers
    }
    sem_post(&shared_memory->mutex);  // Release mutex

    printf("Reader is reading multiple consecutive records\n");
    log_event("Reader started reading");

    add_active_reader(pid); //add the reader's process ID to the active readers array

    int start_record = rand() % record_count;  // Start reading from a random record
    int num_records_to_read = 10;
    if (start_record + num_records_to_read > record_count) {
        num_records_to_read = record_count - start_record;
    }

    printf("Read records:\n");
    for (int i = start_record; i < start_record + num_records_to_read; i++) {
        printf("Customer ID %d, Balance: %d\n", records[i].customer_id, records[i].balance);
    }

    sleep_random_time(10);

    sem_wait(&shared_memory->mutex);  // Acquire mutex to update readers_count
    shared_memory->readers_count--;
    if (shared_memory->readers_count == 0) {
        sem_post(&shared_memory->write_mutex);  // If the last reader, unblock writers
    }
    sem_post(&shared_memory->mutex);  // Release mutex

    remove_active_reader(pid);  //remove the reader's process ID from the active readers array
    log_event("Reader finished reading");

    //calculating average balance
    int remaining_records_sum = 0;
    int remaining_records_count = record_count - num_records_to_read;
    for (int i = start_record + num_records_to_read; i < record_count; i++) {
        remaining_records_sum += records[i].balance;
    }
    double average = (double)remaining_records_sum / remaining_records_count;
    printf("Average balance of records read: %lf\n", average);

    //updating statistics
    time_t end_time;
    time(&end_time);
    sem_wait(&shared_memory->mutex);  // Acquire mutex to protect statistics update
    statistics->num_readers++;
    statistics->total_reader_time += difftime(end_time, start_time);
    statistics->total_records_processed += num_records_to_read;
    sem_post(&shared_memory->mutex);  // Release mutex

    pthread_exit(NULL);
}

void* writer(void* arg) {
    
    time_t start_time;
    time(&start_time);
    pid_t pid = getpid();
    update_statistics_start_time(); 

    sem_wait(&shared_memory->turnstile);  // Wait in line
    sem_post(&shared_memory->turnstile);  // Allow the next in line
    sem_wait(&shared_memory->write_mutex);  // Acquire write_mutex to ensure exclusive access for writing
    shared_memory->writers_count++;
    if (shared_memory->writers_count == 1) {
        sem_wait(&shared_memory->turnstile);  // If the first writer, block readers
    }
    sem_post(&shared_memory->write_mutex);  // Release write_mutex

    printf("Writer is updating the balance of a record\n");
    log_event("Writer started writing");
    add_active_writer(pid); //add the writer's process ID to the active writers array

    int record_to_write = rand() % record_count;
    int amount_to_change = rand() % (records[record_to_write].balance + 1);  //random number between 0 and current balance
    amount_to_change *= (rand() % 2 == 0) ? 1 : -1;  //add or subtract
    records[record_to_write].balance += amount_to_change;
    printf("Changed record: Customer ID %d, New Balance: %d\n", records[record_to_write].customer_id, records[record_to_write].balance);

    sleep_random_time(10);

    sem_wait(&shared_memory->write_mutex);  // Acquire write_mutex to update writers_count
    shared_memory->writers_count--;
    if (shared_memory->writers_count == 0) {
        sem_post(&shared_memory->turnstile);  // If the last writer, unblock readers
    }
    sem_post(&shared_memory->write_mutex);  // Release write_mutex

    remove_active_writer(pid);  //remove the writer's process ID from the active writers array
    log_event("Writer finished writing");

    //updating statistics
    time_t end_time;
    time(&end_time);
    sem_wait(&shared_memory->mutex);  // Acquire mutex to protect statistics update
    statistics->num_writers++;
    statistics->total_writer_time += difftime(end_time, start_time);
    statistics->total_records_processed++;
    sem_post(&shared_memory->mutex);  // Release mutex

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {

    srand(time(NULL));

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <binary_file> num_readers num_writers\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* filename = argv[1];
    int num_readers = atoi(argv[2]);
    int num_writers = atoi(argv[3]);

    //initializing statistics
    initialize_statistics();
    //initializing shared memory
    initialize_shared_memory();

    //load records from the binary file
    record_count = load_records_from_file(filename);

    //creating reader and writer threads
    pthread_t reader_threads[num_readers];
    pthread_t writer_threads[num_writers];

    for (int i = 0; i < num_readers; i++) {
        pthread_create(&reader_threads[i], NULL, reader, NULL);
    }

    for (int i = 0; i < num_writers; i++) {
        pthread_create(&writer_threads[i], NULL, writer, NULL);
    }

    //waiting for threads to finish
    for (int i = 0; i < num_readers; i++) {
        pthread_join(reader_threads[i], NULL);
    }

    for (int i = 0; i < num_writers; i++) {
        pthread_join(writer_threads[i], NULL);
    }

    print_statistics();

    cleanup();

    return 0;
}
