#include "procedures.h"

void sleep_random_time(int);
void log_event(const char*);
void print_active_processes();

void add_active_reader(pid_t );
void remove_active_reader(pid_t );
void add_active_writer(pid_t );
void remove_active_writer(pid_t );