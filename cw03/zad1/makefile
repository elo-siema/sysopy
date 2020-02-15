CC = gcc
CFLAGS = -Wall

all: clean main.o

print_files_stat.o: print_files_stat.c print_files.h
	$(CC) $(CFLAGS) -c -o print_files_stat.o print_files_stat.c

print_files_stat.a: print_files_stat.o
	ar rcs print_files_stat.a print_files_stat.o

main.o: print_files_stat.a
	$(CC) $(CFLAGS) -o main.o main.c print_files_stat.a

clean:
	-rm -rf *.o *.so.* *.so *.a *.gch $(TARGET)