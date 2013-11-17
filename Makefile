all: test.c
	gcc brute.c test.c -o test -O2 -lssl -lcrypto -Wall
clean:
	rm test
