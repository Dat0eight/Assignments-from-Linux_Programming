#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>


#define PATH_NAME "dulieu2.txt"
#define handle_error(msg) do {perror(msg); exit(EXIT_FAILURE);} while(0)
pthread_t thread_id1, thread_id2;

static void *thr_handle(void *args)
{
    int fileDescriptor;
    
    fileDescriptor = open (PATH_NAME,O_RDWR | O_APPEND | O_CREAT, 0777);
    if (fileDescriptor == -1){
        handle_error("open()");
    }
    int i;
    for ( i = 0; i < 500000; i++){
        if (write(fileDescriptor, "A", sizeof("A")) == -1){
            handle_error("write()");
        }
    }

}

int main()
{
    double runTime;
    clock_t beginTime, endTime;
    
    beginTime = clock(); 

    if (pthread_create(&thread_id1, NULL, &thr_handle, NULL) != 0){
        handle_error("pthread_creat()");
    }

    if (pthread_create(&thread_id2, NULL, &thr_handle, NULL) != 0){
        handle_error("pthread_creat()");
    }
    
    pthread_join(thread_id1, NULL);
    pthread_join(thread_id2, NULL);

    endTime = clock();
    printf("Total runtime: %ld\n", endTime - beginTime);

}