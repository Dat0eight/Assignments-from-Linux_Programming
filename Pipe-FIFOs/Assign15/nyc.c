#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


#define handle_error(msg) do {perror(msg); exit(EXIT_FAILURE);} while(0)
#define FIFO_PATH "./messageFIFO"

int main (){
    int fd;
    char buff[256];
    
    // if(mkfifo(FIFO_PATH , 0666) == -1){ //if add this code, It appear error with EEXISTS error code
    //     handle_error("mkfifo()");
    // }
    mkfifo(FIFO_PATH , 0666);
    
    //just transfer message to nym
    while(1){
        printf("Say something to nym:");
        fgets(buff, sizeof(buff), stdin);
            
        fd = open(FIFO_PATH, O_WRONLY, 0666);
        if( fd == -1){
            handle_error("open()");
        } 

        if(write( fd, buff, sizeof(buff)) == -1){
        handle_error("open()");
        }
    }

    return 0;

}