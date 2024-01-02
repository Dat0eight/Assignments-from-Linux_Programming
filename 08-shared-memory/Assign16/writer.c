#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define NAME_PATH "shared_memory_assign16"
#define MEM_SIZE 100

#define handle_error( msg ) do { perror( msg ); exit(EXIT_FAILURE); } while(0)


int main()
{
    int fileDescriptor;
    char *dataAddr;

    fileDescriptor = shm_open( NAME_PATH, O_RDWR | O_CREAT | O_APPEND , 0666 );
    if ( fileDescriptor == -1 ){
        handle_error("shm_open()");
    }

    if( ftruncate(fileDescriptor, MEM_SIZE ) == -1 ){
        handle_error("ftruncate()");
    }

    dataAddr = mmap(0, MEM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fileDescriptor, 0); //if add MAP_PRIVATE instead of MAP_SHARED, reader could not read changed data

    printf("Write something to reader: \n");
    if ( fgets(dataAddr, MEM_SIZE, stdin) == NULL ){
        handle_error("fgets()");
    }

    if ( write( fileDescriptor, dataAddr, MEM_SIZE ) == -1 ){
        handle_error("write()");
    }

    return 0;
}