CC = gcc
CFLAGS = -Wall

all: clean nftw.o stat.o

print_files_nftw.o: print_files_nftw.c print_files.h
	$(CC) $(CFLAGS) -c -o print_files_nftw.o print_files_nftw.c

print_files_nftw.a: print_files_nftw.o
	ar rcs print_files_nftw.a print_files_nftw.o

print_files_stat.o: print_files_stat.c print_files.h
	$(CC) $(CFLAGS) -c -o print_files_stat.o print_files_stat.c

print_files_stat.a: print_files_stat.o
	ar rcs print_files_stat.a print_files_stat.o

nftw.o: print_files_nftw.a
	$(CC) $(CFLAGS) -o nftw.o main.c print_files_nftw.a
stat.o: print_files_stat.a
	$(CC) $(CFLAGS) -o stat.o main.c print_files_stat.a

clean:
	-rm -rf *.o *.so.* *.so *.a *.gch $(TARGET)