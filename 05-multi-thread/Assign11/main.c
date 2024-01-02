//*****************************************************************************************
//*************BaiTap11********************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_t thread_id1, thread_id2;

typedef struct {
    char name[30];
    char dob[30];
    char phone[15];
    char home[15];
} thr_data_t;

static void *thr_handle(void *args) 
{
    pthread_t tid = pthread_self();
    thr_data_t *data = (thr_data_t *)args;

    if (pthread_equal(tid, thread_id1)) {
        printf("I'm thread_id1\n\n");
    } else {
        printf("I'm thread_id2\n");
        printf("Hello Tui la %s, %s, %s, %s\n ", data->name, data->dob, data->phone, data->home);
    }
}

int main(int argc, char const *argv[])
{
    int ret;
    thr_data_t data = {0};

    strncpy(data.name, "Dat Nguyen", sizeof(data.name));                 
    strncpy(data.dob, "sinh nam 2001", sizeof(data.dob));
    strncpy(data.phone, "0999 686868", sizeof(data.phone)); 
    strncpy(data.home, "Quang Tri", sizeof(data.home));

    if (ret = pthread_create(&thread_id1, NULL, &thr_handle, NULL)) {
        printf("pthread_create() error number=%d\n", ret);
        return -1;
    }

    sleep(2);

    if (ret = pthread_create(&thread_id2, NULL, &thr_handle, &data)) {
        printf("pthread_create() error number=%d\n", ret);
        return -1;
    }

    sleep(5);

    return 0;
}
