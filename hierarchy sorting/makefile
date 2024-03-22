# Paths
SRC = ./src
HEADERS = ../headers

# Compiler
CC = gcc

# Compile Options
CFLAGS = -Wall -Werror -g -I$(HEADERS) -lm -std=c99

OBJV0 = $(SRC)/coordinator.o $(SRC)/record.o $(SRC)/mfunctions.o
OBJV1 = $(SRC)/mergers.o $(SRC)/record.o
OBJV2 = $(SRC)/sorters.o $(SRC)/record.o
OBJV3 = $(SRC)/quicksort.o $(SRC)/record.o
OBJV4 = $(SRC)/bubblesort.o $(SRC)/record.o

ARGSV = -i ./testdata/voters500.bin -k 4 -e1 quicksort -e2 bubblesort

$(SRC)/record.o: $(SRC)/record.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC)/mfunctions.o: $(SRC)/mfunctions.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC)/coordinator.o: $(SRC)/coordinator.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC)/mergers.o: $(SRC)/mergers.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC)/sorters.o: $(SRC)/sorters.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC)/quicksort.o: $(SRC)/quicksort.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC)/bubblesort.o: $(SRC)/bubblesort.c
	$(CC) $(CFLAGS) -c $< -o $@


mysort: $(OBJV0) 
	$(CC) $(CFLAGS) $(OBJV0) -o mysort -lm -g
mergers: $(OBJV1) 
	$(CC) $(CFLAGS) $(OBJV1) -o mergers -lm -g
sorters: $(OBJV2) 
	$(CC) $(CFLAGS) $(OBJV2) -o sorters -lm -g
quicksort: $(OBJV3) 
	$(CC) $(CFLAGS) $(OBJV3) -o quicksort -lm -g
bubblesort: $(OBJV4) 
	$(CC) $(CFLAGS) $(OBJV4) -o bubblesort -lm -g


run_sort: mysort mergers sorters quicksort bubblesort
	./mysort $(ARGSV)

clean:
	rm -f $(OBJV0) $(OBJV1) $(OBJV2) $(OBJV3) $(OBJV4) mysort mergers sorters quicksort bubblesort
	rm merger*
	rm sorter*

valgrind_sort: mysort mergers sorters quicksort bubblesort
# valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all -s ./mysort $(ARGSV)
	valgrind --leak-check=full ./mysort $(ARGSV)