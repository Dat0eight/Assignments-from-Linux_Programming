
////////////////////*Chưa tối ưu ở chỗ nhập exit*///////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE);} while (0)
#define LISTEN_BACKLOG 50
#define BUFF_SIZE 256
 
void chatBox(int new_socket_fd)
{
    int numb_read, numb_write;
    char sendBuff[BUFF_SIZE];
    char recvBuff[BUFF_SIZE];

    while(1){
        //remove character if have in sendBuff, recvBuff
        memset(sendBuff, '0', BUFF_SIZE);
        memset(recvBuff, '0', BUFF_SIZE);

        //read data from client
        numb_read = read(new_socket_fd, recvBuff, BUFF_SIZE);
        if(numb_read == -1) {
            handle_error("read()");
        }
        if(strncmp("exit", recvBuff , 4) == 0){
            system("clear");
            break;
        }

        printf("message from client: %s\n", recvBuff);

        printf("please respond the message: ");
        fgets(sendBuff, BUFF_SIZE, stdin);

        //write message to client
        numb_write = write(new_socket_fd, sendBuff, BUFF_SIZE);
        if(numb_write == -1) {
            handle_error("write()");
        }
        if(strncmp("exit", sendBuff, 4) == 0){
            system("clear");
            break;
        }

        sleep(1);
    }
    close(new_socket_fd);
}

void main (int argc, char *argv[] ) //not correct main (char *argv[] , int argc)
{
    int server_fd, newserver_fd;
    int port_no, opt, len;
    struct sockaddr_in server_addr, client_addr;

    //Before creating socket, let's check the correction of command line, inluding 2 agruments
    //>>> ./server <port number>
    //>>> if command line correct -> convert string to integer
    if (argc < 2){
       printf("No port provided! \nCommand line follow this direction: ./server <port number>\n"); 
       exit(EXIT_FAILURE);
    } else port_no = atoi(argv[1]);

    //clear the server_addr and client_addr
    memset(&server_addr , 0 , sizeof(struct sockaddr_in));
    memset(&client_addr , 0 , sizeof(struct sockaddr_in));

    //creating socket
    server_fd = socket(AF_INET , SOCK_STREAM , 0);
    if (server_fd == -1){
        handle_error("socket()");
    }

    //check socket address to know did socket address used
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1){
        handle_error("setsockopt");
    }
    //add address to server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_no);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Assigning a name to a socket
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){ //lỗi chắc
        handle_error("bind()");
    }
    
    if(listen(server_fd, LISTEN_BACKLOG) == -1){
        handle_error("listen()");
    }

    socklen_t addrLength = sizeof(client_addr);

    while(1){
        printf("Server is listening at port : %d\n......\n", port_no);
        newserver_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrLength);
        if(newserver_fd == -1) {
            handle_error("accept()");
        }
        system("clear");
        printf("Server: got connection\n");
            chatBox(newserver_fd);
    }
    
    close (server_fd);
    
}