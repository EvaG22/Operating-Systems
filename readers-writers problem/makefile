# Paths
SRC = src
HEADERS = headers

# Compiler
CC = gcc

# Compile Options
CFLAGS = -Wall -Werror -g -I$(HEADERS) -lpthread -lrt -D_XOPEN_SOURCE=500
TSAN_FLAGS = -fsanitize=thread

GOAL = $(SRC)/sharedmem
OBJV = $(SRC)/sharedmem.o $(SRC)/procedures.o $(SRC)/read_write_proc.o
ARGSV = testdata/accounts50.bin 10 10

$(GOAL): $(OBJV)
	$(CC) $(CFLAGS)$(TSAN_FLAGS) $(OBJV) -o $(GOAL) -lpthread -lrt

run: $(GOAL)
	./$(GOAL) $(ARGSV)

valgrind_run: $(GOAL)
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all -s ./$(GOAL) $(ARGSV)

clean:
	rm -f $(OBJV) $(GOAL) log.txt