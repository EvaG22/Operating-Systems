# Paths
SRC = ./src
HEADERS = ../headers

# Compiler
CC = gcc

# Compile Options
CFLAGS = -Wall -Werror -g -I$(HEADERS) -lm -std=c99

VOTER = main
OBJV = main.o  $(SRC)/voter.o $(SRC)/vhashtable.o $(SRC)/vlist.o 
ARGSV = -f datasets/voters50.csv -b 50

$(VOTER): $(OBJV)
	$(CC) $(CFLAGS) $(OBJV) -o $(VOTER) -lm -g

run_voter : $(VOTER)
	./$(VOTER) $(ARGSV)

valgrind_voter:	$(VOTER)
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all -s ./$(VOTER) $(ARGSV)

clean:
	rm -f $(OBJV) $(VOTER)
