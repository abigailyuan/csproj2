CC = gcc -std=gnu99

all: server

server: server.h server.c 
	$(CC) -g -o server server.c

test_crypto: crypto/sha256.h crypto/sha256.c crypto/sha256_test.c
	$(CC) -g -o sha256_test crypto/sha256.c crypto/sha256_test.c

test_uint256: uint256.h uint256_test.c
	$(CC) -g -o uint256_test uint256_test.c

run_test_client: test_crypto test_uint256
	./sha256_test
	./uint256_test

clean:
	rm -rf ./sha256_test ./uint256_test *.o
	rm ./server

.PHONY = run_test_client clean
