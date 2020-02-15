#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

unsigned char* gen_rdm_bytestream(size_t num_bytes)
{
	unsigned char* stream = (unsigned char*)malloc(num_bytes);
	size_t i;

	for (i = 0; i < num_bytes; i++)
	{
		stream[i] = rand();
	}

	return stream;
}


void generate(char* file, int records, size_t size) {
	int file_descriptor;

	int f = creat(file, FILE_MODE);
	if (f < 0) {
		perror("Can't create file");
		exit(1);
	}

	for (int i = 0; i < records; i++) {
		unsigned char* buf = gen_rdm_bytestream(size);
		if (write(f, buf, size) != size) {
			perror("Can't write to file");
			exit(1);
		}
		free(buf);
	}
	if (close(f) != 0) {
		perror("Can't close file");
		exit(1);
	}
}


void sort_sys(char* file, int records, size_t size) {
    int file_source=open(file,O_RDWR);
    if(file_source<0){
        perror("Can't open file "); 
        exit(1);
    }
    int offset=sizeof(char)*size;

    char* insert_buf=calloc(size,sizeof(char));
    char* swap_buf=calloc(size,sizeof(char));

    for(int i=1; i<records; i++){

        if(lseek(file_source,i*offset,SEEK_SET)==-1){
            perror("Can't seek");
        exit(1);
        }
        
        if(read(file_source,insert_buf,size)!=size){
            perror("Can't read file");   
        exit(1);
        }

        int j = 0;
        for(j=i-1; j>=0; j--){

            if(lseek(file_source,j*offset,SEEK_SET)==-1){
                perror("Can't seek"); 
                exit(1);
            }
                
            if(read(file_source,swap_buf,size)!=size){
                perror("Can't read file");
                exit(1);
            }
            if(insert_buf[0] >= swap_buf[0]) break;

            if(write(file_source,swap_buf,size) != size){
                perror("Can't write swap buffer to file"); 
                exit(1);
            }     
        }
        if(lseek(file_source,(j+1)*offset,SEEK_SET)==-1){
            perror("Can't seek"); 
            exit(1);
        }
        if(write(file_source,insert_buf,size) != size){
            perror("Can't write insert buffer to file");
            exit(1);
        }
    }

    free(insert_buf);
    free(swap_buf);
    if(close(file_source)!=0){
        perror("Can't close file");
        exit(1);
    }
}

void copy_sys(char* fileFrom, char* fileTo,  int records, size_t size) {
    int file_source;
    int file_copy;

    file_source=open(fileFrom,O_RDONLY);
    if(file_source<0){
        perror("Can't open file ");
        exit(1);
    }
    file_copy=creat(fileTo,FILE_MODE);
    if(file_copy<0){
        perror("Can't create file ");
        exit(1);
    }
    char* buf=calloc(size,sizeof(char));
    int read_num=read(file_source,buf,size);
    while(read_num>0){
        if(write(file_copy,buf,read_num)!=read_num){
            perror("Can't write to file");
            exit(1);
        }    
        read_num=read(file_source,buf,size);
    }

    if(read_num<0){
        perror("Can't read file");
        exit(1);
    }
    free(buf);
    if(close(file_source)!=0){
        perror("Can't close source file");
        exit(1);
    }    
    if(close(file_copy)!=0){
        perror("Can't close copy file");
        exit(1);
    }    
}

void sort_lib(char* file, int records, size_t size) {
    FILE *file_source = fopen(file, "rw+");
    if(file_source==NULL){
        perror("Can't open file "); 
        exit(1);
    }
    int offset=sizeof(char)*size;

    char* insert_buf=calloc(size,sizeof(char));
    char* swap_buf=calloc(size,sizeof(char));

    for(int i=1; i<records; i++){

        if(fseek(file_source,i*offset,0)!=0){
            perror("Can't seek");
        exit(1);
        }
        
        if(fread(insert_buf,sizeof(char),size,file_source)!=size){
            perror("Can't read file");   
        exit(1);
        }

        int j = 0;
        for(j=i-1; j>=0; j--){

            if(fseek(file_source,j*offset,0)!=0){
                perror("Can't seek"); 
                exit(1);
            }
                
            if(fread(swap_buf,sizeof(char),size,file_source)!=size){
                perror("Can't read file");
                exit(1);
            }
            if(insert_buf[0] >= swap_buf[0]) break;

            if(fwrite(swap_buf, sizeof(char),size, file_source) != size){
                perror("Can't write swap buffer to file"); 
                exit(1);
            }     
        }
        if(fseek(file_source,(j+1)*offset,0)!=0){
            perror("Can't seek"); 
            exit(1);
        }
        if(fwrite(insert_buf, sizeof(char),size, file_source) != size){
            perror("Can't write insert buffer to file");
            exit(1);
        }
    }

    free(insert_buf);
    free(swap_buf);
    if(fclose(file_source)!=0){
        perror("Can't close file");
        exit(1);
    }
}

void copy_lib(char* fileFrom, char* fileTo,  int records, size_t size) {
    FILE *file_source=fopen(fileFrom,"r");
    if(file_source==NULL){
        perror("Can't open file ");
        exit(1);
    }
    FILE *file_copy=fopen(fileTo,"w+");
    if(file_copy==NULL){
        perror("Can't create file ");
        exit(1);
    }
    char* buf=calloc(size,sizeof(char));

    size_t read_num=fread(buf, sizeof(char), size, file_source);
    while(read_num>0){
        if(fread(buf, sizeof(char), read_num, file_copy)!=read_num){
            perror("Can't write to file");
            exit(1);
        }    
        read_num=fread(buf, sizeof(char), size, file_source);
    }

    if(read_num<0){
        perror("Can't read file");
        exit(1);
    }
    free(buf);
    if(fclose(file_source)!=0){
        perror("Can't close source file");
        exit(1);
    }    
    if(fclose(file_copy)!=0){
        perror("Can't close copy file");
        exit(1);
    }    
}

char* command_str[] = {
    "generate",
    "sort",
    "copy"
};

int command_offset[] = {
    0,
    0,
    1
};

typedef enum {
    GENERATE,
    SORT,
    COPY,
    CMD_DEFAULT
} command;

char* fsource_str[] = {
    "sys",
    "lib"
};

typedef enum {
    SYS,
    LIB, 
    FSC_DEFAULT
} fsource;

int main(int argc, char ** argv) {
    srand(time(NULL));
    command comm = CMD_DEFAULT;
    char * file1;
    char * file2;
    int records = 0;
    int size = 0;
    fsource fsc = FSC_DEFAULT;

    if (argc < 4) {
        perror("Too few arguments");
        exit(1);
    }
    if (argc > 7) {
        perror("There should be no more than 6 arguments");
        exit(1);
    }

    //command:
    if(strcmp(argv[1], command_str[GENERATE])==0) comm = GENERATE;
    if(strcmp(argv[1], command_str[SORT])==0)     comm = SORT;
    if(strcmp(argv[1], command_str[COPY])==0)     comm = COPY;

    //file 1:
    file1 = argv[2];

    //file 2 (optional):
    file2 = argv[3];

    //no of records:
    records = strtol(argv[3 + command_offset[comm]], NULL, 0);

    //size of record:
    size = strtol(argv[4 + command_offset[comm]], NULL, 0);

    //function source:
    if(5 + command_offset[comm] < argc){
        if(strcmp(argv[5 + command_offset[comm]], fsource_str[SYS])==0) fsc = SYS;
        if(strcmp(argv[5 + command_offset[comm]], fsource_str[LIB])==0) fsc = LIB;
    }
    //validate common params
    if(!records) {
        perror("Number of records provided in an incorrect format.");
        exit(1);
    }
    if(!size) {
        perror("Size of a record provided in an incorrect format.");
        exit(1);
    }

    switch(comm) {
        case GENERATE:
            generate(file1, records, size);
            break;
        case SORT:
            switch(fsc) {
                case SYS:
                    sort_sys(file1, records, size);
                    break;
                case LIB:
                    sort_lib(file1, records, size);
                    break;
                default:
                    perror("Last parameter is incorrect.");
                    exit(1);
            }
            break;
        case COPY:
            switch(fsc) {
                case SYS:
                    copy_sys(file1, file2, records, size);
                    break;
                case LIB:
                    copy_lib(file1, file2, records, size);
                    break;
                default:
                    perror("Last parameter is incorrect.");
                    exit(1);
            }
            break;
        default:
            perror("Incorrect ");
            exit(1);
    }
    return 0;
}