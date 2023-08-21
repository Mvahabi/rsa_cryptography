CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra -g $(shell pkg-config --cflags gmp)
LDFLAGS = -lm $(shell pkg-config --libs gmp) 

all: encrypt decrypt keygen

encrypt: encrypt.o numtheory.o randstate.o rsa.o
	$(CC) $(LDFLAGS) -o encrypt encrypt.o numtheory.o randstate.o rsa.o

encrypt.o: encrypt.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -c encrypt.c 

decrypt: decrypt.o numtheory.o randstate.o rsa.o
	$(CC) $(LDFLAGS) -o decrypt decrypt.o numtheory.o randstate.o rsa.o

decrypt.o: decrypt.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -c decrypt.c

keygen: keygen.o numtheory.o randstate.h rsa.h 
	$(CC) $(LDFLAGS) -o keygen keygen.o numtheory.o randstate.o rsa.o

keygen.o: keygen.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -c keygen.c

clean:
	rm -rf *.o encrypt decrypt keygen

format:
	clang-format -i -style=file *.[ch]
	
