CC = gcc
CFLAGS = -Wall

all: static.o shared.o dynamic.o

fibonacci_lib.o: fibonacci_lib.c fibonacci_lib.h
	$(CC) $(CFLAGS) -c -o fibonacci_lib.o fibonacci_lib.c

fibonacci_lib.a: fibonacci_lib.o
	ar rcs fibonacci_lib.a fibonacci_lib.o

fibonacci_lib.so: fibonacci_lib.c fibonacci_lib.h
	$(CC) $(CFLAGS) -c -fPIC -o fibonacci_lib.o fibonacci_lib.c
	$(CC) $(CFLAGS) -shared  -fPIC -o fibonacci_lib.so \
					fibonacci_lib.o

static.o: static_shared.c fibonacci_lib.a
	$(CC) $(CFLAGS) -o static.o static_shared.c fibonacci_lib.a

shared.o: static_shared.c fibonacci_lib.so
	$(CC) $(CFLAGS) -o shared.o static_shared.c -L. ./fibonacci_lib.so

dynamic.o: dynamic.c fibonacci_lib.so
	$(CC) $(CFLAGS) -o dynamic.o dynamic.c -ldl

clean:
	-rm -rf *.o *.so.* *.so *.a *.gch $(TARGET)