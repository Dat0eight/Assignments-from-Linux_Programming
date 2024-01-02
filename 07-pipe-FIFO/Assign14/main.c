#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#define handle_error(error_msg) do {perror(error_msg); exit(EXIT_FAILURE);} while(0)
#define BUFF_SIZE 256
char *msg1 = "Hello world #1";
char *msg2 = "Hello world #2";
char *msg3 = "Hello world #3";
char *msg4 = "Hello world #4";

int main (int argc, char *argv[]){
    int pipefd[2];
    char in_buff[BUFF_SIZE];
    int child_pid, pipe_value, num_read;
    
    //creating a pipe
    pipe_value = pipe(pipefd);
    if(pipe_value == -1){
        handle_error("pipe()");
    }
    //create fork
    child_pid = fork();
    if(child_pid == 0){
        printf("I'm Children process.\n"); //writer
        if(close(pipefd[0]) == -1){
            handle_error("close()");
        }
        
        write(pipefd[1], msg1 , BUFF_SIZE);
        write(pipefd[1], msg2 , BUFF_SIZE);
        write(pipefd[1], msg3 , BUFF_SIZE);
        write(pipefd[1], msg4 , BUFF_SIZE);
        while(1);
        close(pipefd[1]);

    } else if (child_pid > 0) {     //if (child_pid == -1)
        printf("I'm Parent process.\n"); //reader
        if(close(pipefd[1]) == -1){
            handle_error("close()");
        }
        //block here untill read some data
        while(1){
            /*Các trường hợp sau làm cho hết block:
            1. Đọc xong dữ liệu từ write
            2. Không có dữ liệu, tức là kh có write được call
            3. Khi đóng đầu write->kiểu nó sẽ hiểu là kh bao giờ có dữ liệu nữa(do đầu write bị đóng rồi)*/
            num_read = read(pipefd[0], in_buff, BUFF_SIZE); 
            if(num_read == -1){
                handle_error("read()");
                // break;
            } else if(num_read == 0){ //khi dong dau ghi
                printf("write closed\n");
                break;
            } else {
                printf("msg: %s\n", in_buff); //bthg ra ra nhu the nay
            }
        };

    } else {
        handle_error("fork()");
        }

    return 0;
}