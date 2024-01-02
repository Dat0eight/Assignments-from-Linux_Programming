#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define handle_error(msg) do{perror(msg); exit(EXIT_FAILURE);} while(0)
#define BUFF_SIZE 256


void chatBox(int new_socket_fd){
    char recv_buff[BUFF_SIZE];
    char send_buff[BUFF_SIZE];
    int numb_read, numb_write;

    while(1){
        memset(&recv_buff, '0' , BUFF_SIZE); //why don't put out of the while loop
        memset(&send_buff, '0' , BUFF_SIZE);

        printf("please send message:...");
        fgets(send_buff, BUFF_SIZE, stdin);

        if(strncmp("exit", send_buff, 4) == 0){
            printf("Client exit... \n");
            break;
        }
        //>>>firstly, send message to server 
        numb_write = write(new_socket_fd, send_buff, BUFF_SIZE);
        if(numb_write == -1 ){
            handle_error("write()");
            exit(EXIT_FAILURE);
        }
        //>>then, receive message from server
        numb_read = read(new_socket_fd, recv_buff, BUFF_SIZE);
        if(numb_read == -1){
            handle_error("read()");
            exit(EXIT_FAILURE);
            break;
        }
        printf("Message from server: %s", recv_buff);
    }
    close(new_socket_fd);

}
void main(int argc , char *argv[]){
    int client_fd, port_no;
    struct sockaddr_in server_addr, client_addr;
    
    if (argc < 3){
        printf("Command line follow this direction:\n ./client <IP address> <port number>\n");
        exit(1);
    }
    
    //port number is define like a string >> convert to integer
    port_no = atoi(argv[2]); 

    memset(&client_addr, '0', sizeof(struct sockaddr_in ));
    memset(&server_addr, '0', sizeof(struct sockaddr_in ));

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd == -1){
        handle_error("socket()");
        exit(EXIT_FAILURE);
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port_no);
    client_addr.sin_addr.s_addr = INADDR_ANY;

    socklen_t addrLength;
    addrLength = sizeof(client_addr);

    if(connect(client_fd, (struct sockaddr *)&client_addr, addrLength) == -1){
        handle_error("connect()");
        exit(EXIT_FAILURE);
    }
    chatBox(client_fd);

}