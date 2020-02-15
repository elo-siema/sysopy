CC = gcc
CFLAGS = -Wall -std=c11 -g 

all: clean server.o client.o

server.o: queue.h server.c
	$(CC) $(CFLAGS) -o server.o server.c queue.c -lrt

client.o: queue.h server.c
	$(CC) $(CFLAGS) -o client.o client.c queue.c -lrt

clean:

	-rm -rf *.o *.so.* *.so *.a *.gch $(TARGET)
