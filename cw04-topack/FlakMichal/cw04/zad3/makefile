CC = gcc
CFLAGS = -Wall -g

all: clean main.o

main.o: 
	$(CC) $(CFLAGS) -o main.o main.c

clean:
	-rm -rf *.o *.so.* *.so *.a *.gch $(TARGET)