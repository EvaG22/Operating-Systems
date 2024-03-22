#include "../headers/read_write_proc.h"


void sleep_random_time(int max_duration) {
    int sleep_time = rand() % (max_duration + 1);
    usleep(sleep_time * 1000000);
}

void log_event(const char* event) {
    FILE* log_file = fopen("log.txt", "a");
    if (log_file != NULL) {
        fprintf(log_file, "%s\n", event);
        fclose(log_file);
    }
}

void print_active_processes() {
    printf("Active Readers: ");
    for (int i = 0; i < MAX_PROCESS_IDS; i++) {
        if (shared_memory->active_readers[i] != 0) {
            printf("%ld ", shared_memory->active_readers[i]);
        }
    }
    printf("\n");

    printf("Active Writers: ");
    for (int i = 0; i < MAX_PROCESS_IDS; i++) {
        if (shared_memory->active_writers[i] != 0) {
            printf("%ld ", shared_memory->active_writers[i]);
        }
    }
    printf("\n");
}

void add_active_reader(pid_t pid) {
    for (int i = 0; i < MAX_PROCESS_IDS; i++) {
        if (shared_memory->active_readers[i] == 0) {
            shared_memory->active_readers[i] = pid;
            break;
        }
    }
}

void remove_active_reader(pid_t pid) {
    for (int i = 0; i < MAX_PROCESS_IDS; i++) {
        if (shared_memory->active_readers[i] == pid) {
            shared_memory->active_readers[i] = 0;
            break;
        }
    }
}

void add_active_writer(pid_t pid) {
    for (int i = 0; i < MAX_PROCESS_IDS; i++) {
        if (shared_memory->active_writers[i] == 0) {
            shared_memory->active_writers[i] = pid;
            break;
        }
    }
}

void remove_active_writer(pid_t pid) {
    for (int i = 0; i < MAX_PROCESS_IDS; i++) {
        if (shared_memory->active_writers[i] == pid) {
            shared_memory->active_writers[i] = 0;
            break;
        }
    }
}