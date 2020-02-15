CC = gcc
CFLAGS = -Wall -g
LIBS=-lpthread

all: clean main

main.o: main.c
	$(CC) $^ -o $@ $(CFLAGS) $(LIBS)

clean:
	-rm -rf *.o *.so.* *.so *.a *.gch $(TARGET)