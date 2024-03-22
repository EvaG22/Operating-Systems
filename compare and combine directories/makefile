# Paths
SRC = ./src
HEADERS = ../headers

# Compiler
CC = gcc

# Compile Options
CFLAGS = -Wall -Werror -g -I$(HEADERS) -lm -std=c99 -D_POSIX_C_SOURCE=200809L

COMPARE = cmpcats
OBJV = cmpcats.o  $(SRC)/cmp_functions.o $(SRC)/create_new.o $(SRC)/linked_list.o
ARGSV = -d dirA dirB -s dirC 

$(COMPARE): $(OBJV)
	$(CC) $(CFLAGS) $(OBJV) -o $(COMPARE) -lm -g

run_cmp: $(COMPARE)
	./$(COMPARE) $(ARGSV)

valgrind_cmp: $(COMPARE)
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all -s ./$(COMPARE) $(ARGSV)

clean:
	rm -rf $(OBJV) $(COMPARE) dirC
