#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define handle_error(msg) do {perror(msg); exit(EXIT_FAILURE);} while(0)
#define PATH_NAME "dulieu.txt" //để như này mới tạo file có quyền truy cập được >>>> có sự khác biệt giữa "./dulieu.txt" và "dulieu.txt"

char buff[] = "Hi toi dang lam bai tap APPEND!";

int main(){
    int fd;
    //open file dulieu.txt
    fd = open(PATH_NAME, O_RDWR | O_APPEND | O_CREAT , 0777 ); //try to delete the O_APPEND flags to see how O_APPEND flags work

    //write buff data to dulieu.txt
    if (fd == -1){
        handle_error("write()");
    }
    printf("Open successfully file!\n");
    
    if (write (fd , buff , strlen(buff)) == -1){
        handle_error("write()");
    }
    printf("Write data to dulieu.txt successfully!\n");

    //testing seek action
    if(lseek(fd , 1 ,SEEK_SET) == -1){
        handle_error("lseek()");
    }
    if (write (fd , "This seek action" , strlen("This seek action")) == -1){
        handle_error("write()");
    }
    printf("take seek action successfully!\n");

    return 0;
}