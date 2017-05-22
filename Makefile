CC = gcc -std=gnu99

## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC =		server2.c sha256.c work.c
OBJ =		server2.o sha256.o work.o
EXE = 		server

## Top level target is executable.
$(EXE):	$(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -lm -lpthread


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ)

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE)

## Dependencies
sha256.o: sha256.h
server.o:	server.h
work.o: work.h

all: run_test_client clean
	./uint256_test

.PHONY = run_test_client clean
