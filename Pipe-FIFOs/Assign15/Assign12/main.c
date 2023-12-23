#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t inputReadyCond = PTHREAD_COND_INITIALIZER;

//biến bên dưới được tạo để nhằm mục đích lấy điều kiện cho vòng lặp
int inputReadyVar = 1; 

typedef struct  {               //typedef struct và struct có sự khác biệt
    char name[30];
    char dateOfBirth[30];
    char homeLand[30];
} inforStudent;

//Thread1: Nhập dữ liệu từ bàn phím
static void *handle_th1(void *args)
{       
    pthread_mutex_lock(&lock); 
    
    while(inputReadyVar != 1){ 
    pthread_cond_wait(&inputReadyCond , &lock);                
    }
   
    inforStudent *sinhVien = (inforStudent*)args;

    // inforStudent *sinhVien = malloc(sizeof(inforStudent));
    // inforStudent *sinhVien = (inforStudent *)malloc(sizeof(inforStudent));

    // if (sinhVien == NULL) {
    // fprintf(stderr, "Khong the cap phat bo nho\n");
    // return 1;
    // }

    printf("Nhap Ho va ten: ");
    fgets(sinhVien->name , sizeof(sinhVien->name) , stdin);
    sinhVien->name[strcspn(sinhVien->name, "\n")] = '\0';

    printf("Nhap ngay sinh: ");
    fgets(sinhVien->dateOfBirth , sizeof(sinhVien->dateOfBirth) , stdin);
    sinhVien->dateOfBirth[strcspn(sinhVien->dateOfBirth, "\n")] = '\0';
    
    printf("Nhap que quan: ");
    fgets(sinhVien->homeLand , sizeof(sinhVien->homeLand ) , stdin);
    sinhVien->homeLand[strcspn(sinhVien->homeLand , "\n")] = '\0';

    inputReadyVar = 2; //chạy xong thread1 thì inputReady = 1 -> kết thúc vòng lặp trên
    sleep(2);
    pthread_cond_signal(&inputReadyCond);
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

//Thread2: Ghi thông tin vào file dulieu.txt
static void *handle_th2(void *args)
{
    pthread_mutex_lock(&lock);
    while(inputReadyVar != 2){
        pthread_cond_wait(&inputReadyCond , &lock);
    }

    //Mở tệp để đọc t 
    int fd;
    fd = open("dulieu.txt", O_RDWR | O_APPEND | O_CREAT , 0777); //0777, 0666 dãy quyền truy cập (file permissions )
    if (fd == -1) {
        perror("Error opening file\n");
        // return 1;
    } else{
        printf("Open successful\n");
    }
    
    //bây giờ ghi dữ liệu vào tệp
    inforStudent *sinhVien = (inforStudent*)args;
    char buff[300];


    sprintf(buff, "Ten - Ngay sinh - Que quan: %s - %s - %s \n", sinhVien->name, sinhVien->dateOfBirth, sinhVien->homeLand);
    
    ssize_t result = write(fd , buff , strlen(buff));
    if (result == -1) {
        perror("Error writing to file\n");
        close(fd);
    } else {
        printf("Write successful, %zd bytes written\n", result);
    }
    close (fd);
    inputReadyVar = 3;
    sleep(5);
    pthread_cond_signal(&inputReadyCond);
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);

}

//Đọc dữ liệu vừa ghi được và in ra màn hình, sau đó thông báo cho Thread 1 tiếp tục nhập
static void *handle_th3(void *args)
{
    pthread_mutex_lock(&lock);
    while(inputReadyVar != 3){
        pthread_cond_wait(&inputReadyCond , &lock);
    }
    inforStudent *sinhVien = (inforStudent*)args;
    char buff[300];
    int fd;

    sprintf(buff, "Ten - Ngay sinh - Que quan: %s - %s - %s \n", sinhVien->name, sinhVien->dateOfBirth, sinhVien->homeLand);
    printf("%s", buff);
    
    
    inputReadyVar = 1;
    pthread_cond_signal(&inputReadyCond);
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);

}

int main (void){
    
    int rel;
    pthread_t thread_id1, thread_id2, thread_id3;
    inforStudent person;
    while(1){
        printf ("Chuong trinh cua Dat\n");
    
        if(rel = pthread_create(&thread_id1 , NULL, &handle_th1, &person)){
        printf("pthread_create() error number = %d\n", rel);
        return -1;
        }
        
        if(rel = pthread_create(&thread_id2 , NULL, &handle_th2, &person)){
        printf("pthread_create() error number = %d\n", rel);
        return -1;
        }
        
        if(rel = pthread_create(&thread_id3 , NULL, &handle_th3, &person)){
        printf("pthread_create() error number = %d\n", rel);
        return -1;
        }    
        
        pthread_join(thread_id1, NULL);
        pthread_join(thread_id2, NULL);
        pthread_join(thread_id3, NULL);
    }
    
    return 0;
}