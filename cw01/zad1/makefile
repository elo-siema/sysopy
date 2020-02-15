CC = gcc
CFLAGS = -Wall

all: bubblesort fibonacci fibonacci_rec sito

bubblesort: bubblesort.c
	$(CC) $(CFLAGS) -o bubblesort bubblesort.c

fibonacci: fibonacci.c
	$(CC) $(CFLAGS) -o fibonacci fibonacci.c

fibonacci_rec: fibonacci_rec.c
	$(CC) $(CFLAGS) -o fibonacci_rec fibonacci_rec.c

sito: sito.c
	$(CC) $(CFLAGS) -o sito sito.c

clean:
	-rm -rf *.o $(TARGET)

test:
	for file in bubblesort fibonacci fibonacci_rec sito ; do \
        $(CC) $(CFLAGS) -o $$file.o $$file.c ; \
        $(CC) $(CFLAGS) -o $$file.o1.o $$file.c -O1 ; \
        $(CC) $(CFLAGS) -o $$file.o2.o $$file.c -O2 ; \
        $(CC) $(CFLAGS) -o $$file.o3.o $$file.c -O3 ; \
    done