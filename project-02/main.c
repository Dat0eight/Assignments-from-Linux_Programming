#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>


#define handle_error(msg) do { perror (msg); exit (EXIT_FAILURE); } while (1) /*Macro handle error*/

#define CMD_LINE_SIZE 64        /*Max size of command-line in chat app*/
#define LISTEN_BACKLOG 10       /*Max number of queue*/
#define MSG_SIZE 1024           /*Max size of message from peer connection to others*/
#define MY_IP "192.168.205.129" /*Your public IP, you SHOULD MODIFY this IP (put your IP here, if not program appear error)*/

typedef struct {
    int ID;
    int savedPortNo;
    char IP[INET_ADDRSTRLEN];
    int savedFileDes;
} peerInfo;                     /*peerInfo struct data used for storing IP, port number, file descriptor, ID of peer connections*/

peerInfo saveConnectedClientInfo[LISTEN_BACKLOG];   /*lưu thông tin client sau khi kết nối thành công -> để dùng cho option 5,6,7*/
int numberOfPeerGlo;                                /*An integer to keep track of the number of connected peers*/
int pthreadCreatSerVal, pthreadCreatClientVal;
pthread_t clientThreadID, serverThreadID;

//function prints a list of available commands.
void displayCommandOptions(){
    printf("******************************Chat application******************************\n");
    printf("1.help _____________________________________________________  Manual command\n");
    printf("2.myip __________________________________________ IP-address of this process\n");
    printf("3.myport _________________________ Port is listening for incoming connection\n");
    printf("4.connect <destination> <port no> ________ Establishing a new TCP connection\n");
    printf("5.list _____________________________________________ List of all connections\n");
    printf("6.terminate <connection ID> ____________ Terminating the specific connection\n");
    printf("7.send <connection ID> <message> ________ Sending message to another process\n");
    printf("8.exit ____________________ Close all connections and terminate this process\n");
    printf("******************************________________******************************\n\n\n");
}

//function determines the option selected based on the command-line input
int choosingOption(char **argv){
    int i = 0;

    if ( strcmp(argv[0], "help") == 0 ) return 1;
    if ( strcmp(argv[0], "myip") == 0 ) return 2;
    if ( strcmp(argv[0], "myport") == 0 ) return 3;
    if ( strcmp(argv[0], "connect") == 0 ) return 4;
    if ( strcmp(argv[0], "list") == 0 ) return 5;
    if ( strcmp(argv[0], "terminate") == 0 ) return 6;
    if ( strcmp(argv[0], "send") == 0 ) return 7;
    if ( strcmp(argv[0], "exit") == 0 ) return 8;
}

/**
* @description: This function will split command-line into argv array.
* @param cmd_argument is the string that input from shell
* @return None
*/
void parseCommand (char *cmd_argument, char **argv){
    int i = 0;

    //khởi tạo argv là NULL
    while( i < CMD_LINE_SIZE ){
        argv[i] = NULL;
        i++;
    }

    //split command-line then pass to argv
    i = 0;
    argv[i] = strtok (cmd_argument, " ");
    while ( argv[i] != NULL ){
        i++;
        argv[i] = strtok (NULL, " ");
    }
}

void getIPAddress(){
    printf("Public IP: %s\n", MY_IP);
}

void getPortNo(char **argv){
    int portNo = atoi(argv[1]);
    printf("Port no: %d\n", portNo);
}

/** @Description: hàm lưu giá trị địa chỉ của connected-peer vào saveConnectedClientInfo
 *  @param numberOfPeerGlo: đếm giá trị kết nối, connectionAddr: địa chỉ của connected-peer, fileDescriptor: chưa dùng
 *  @return None
*/
void saveConnections( struct sockaddr_in connectionAddr ){
    //save client connected into buffer
    saveConnectedClientInfo[numberOfPeerGlo].ID = numberOfPeerGlo;

    /*Hiện tại, địa chỉ gồm port no, IP đều được lưu ở dạng network byte order. Cần chuyển sang host byte order*/
    if ( inet_ntop(AF_INET, &connectionAddr.sin_addr.s_addr, saveConnectedClientInfo[numberOfPeerGlo].IP , INET_ADDRSTRLEN) == NULL ){ //storing IP
        handle_error("inet_ntop in saveConnections\n");
    } 
    saveConnectedClientInfo[numberOfPeerGlo].savedPortNo = ntohs(connectionAddr.sin_port);  //storing port number
    numberOfPeerGlo++;                                                                      //next time connection: numberOfPeerGlo + 1
}

/**
 * @Description: Hàm thực hiện gửi tin nhắn thông qua ID hiển thị trên danh sách các kết nối. 
 * @param argv: đối số được tách từ user interface.
 * @return None.
*/
void sendMessage(char **argv){ ///HEREEEEEEEE
    int numRead, numWrite;
    char sendBuff[MSG_SIZE];
    char IDBufff[10];                                   // chuỗi lưu ID
    int sendID;                                         //message sẽ được gửi đến địa chỉ có ID này

    //remove data if have
    memset (IDBufff, '0', 10);
    memset (sendBuff, '0', MSG_SIZE);
    
    strncpy(IDBufff, argv[1], 2);                       //copy ID dạng chuỗi vào buffer
    sendID = atoi(IDBufff);                             //convert sang int
    if (sendID < 0 || sendID >= numberOfPeerGlo){       //check ID available
        printf("Invalid Connection!\n");
        return;
    }

    strncpy(sendBuff, argv[2], MSG_SIZE);               //Upgrade ở đây cho nó xịn (hiện tại message gửi đi kh thể chứa blank-space)

    numWrite = write (saveConnectedClientInfo[sendID].savedFileDes, sendBuff, MSG_SIZE); //send msg

    if ( numWrite == -1) {                              //handle_error ("write in sendMessage"): for debugging only
        printf("Maybe ID does not exist\n\n");
        return;
    }
 
    printf("Message sent to ID: %d successfully\n\n", sendID); 
}
/**
 * @Description: Hàm thực hiện nhận tin nhắn thông qua ID hiển thị trên danh sách các kết nối. Tạo một vòng lặp để có thể liên tục
 * nhận tin nhắn.
 * @param None.
 * @return None.
*/
void *receiveMessage(){ ///HEREEEEEEEE void *socketFD
    int numRead;
    char receiveBuff[MSG_SIZE];
    // sendID = 0;
    // int castedSocketFD = (int)socketFD;
    
    while(1){
        memset (receiveBuff, '0', MSG_SIZE);                    //remove data if have

/****************************************************************************************************************************/
//          Why this: saveConnectedClientInfo[numberOfPeerGlo-1].savedFileDes
//          - Vì khi lưu thì (numberOfPeerGlo + 1) 
//          - Hiện tại, mỗi Chat-app có thể kết nối đến nhiều chat-app khác nhau, nhưng chỉ accept được duy nhất một chat-app.
//          Nên hiện tại chức năng này có thể đáp ứng được.
/*****************************************************************************************************************************/

        numRead = read (saveConnectedClientInfo[numberOfPeerGlo-1].savedFileDes, receiveBuff, MSG_SIZE);
        if ( numRead != -1 && receiveBuff[1] != '0' ) {
            /*Why: 
                - Bỏ handle_error: cơ chế là hàm reveiveMessage này luôn receive, nếu để handle_error ở đây sẽ làm ngắt vòng lặp
                - Tại sao receiveBuff[1] != '0': Nếu terminate mà kh để thêm điều kiện này thì sẽ dẫn đến hiện tượng receive liên tục chuỗi '000...000' */

            printf("\nMessage receive from IP: %s\n", saveConnectedClientInfo[numberOfPeerGlo-1].IP);
            printf("Sender's port: %d\n",saveConnectedClientInfo[numberOfPeerGlo-1].savedPortNo);
            printf("Message:< %s >\n\n", receiveBuff );
          }      
    }
}
/**
 * @Description: creating the server, lưu trữ ID, IP, port no vào buffer 
 * @param arg: đối số từ commmand-line sau khi parse command
 * @return none
*/
void *createServer( void *arg ){ 
    char **argv = (char **)arg; 
    int newSocketFD; //socketFD, 
    int len, portNo;
    struct sockaddr_in serverAddr;

    portNo = atoi(argv[1]);                             //convert port no (string) -> port no (int)

    saveConnectedClientInfo[numberOfPeerGlo].savedFileDes = socket( AF_INET, SOCK_STREAM, 0);
    if ( saveConnectedClientInfo[numberOfPeerGlo].savedFileDes == -1 ) handle_error("socket()");
    
    //set server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNo);                //convert port no (int) -> network byte order
    serverAddr.sin_addr.s_addr = inet_addr(MY_IP);      // inet_addr(atoi(MY_IP));

    //Assign a name to a socket
    if ( bind (saveConnectedClientInfo[numberOfPeerGlo].savedFileDes, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1){
        handle_error("bind()");
    }

    if ( listen ( saveConnectedClientInfo[numberOfPeerGlo].savedFileDes, LISTEN_BACKLOG ) == -1 ){
        handle_error("listen()");
    }

    socklen_t addrLen = sizeof(serverAddr);

    printf("server is listening at port: %d\n", portNo);

    while(1){
        newSocketFD = accept ( saveConnectedClientInfo[numberOfPeerGlo].savedFileDes, (struct sockaddr*)&serverAddr, &addrLen );
        if ( newSocketFD == -1) {                       //handle_error("accept() in create server"): for debugging only
            return NULL;
        }
        saveConnections( serverAddr );
        /*Why numberOfPeerGlo argument of saveConnections is numberOfPeerGlo:
        Là bởi vì sau khi client thực hiện kết nối xong, numberOfPeerGlo value đã được tính, value này đồng nhất trong main func
        */
       
       //sau khi được kết nối, port number in ra ở câu Accepted connection... through port no: port này là cái port của client kết nối
        printf("Accepted connection from IP: %s through port no: %d\n", saveConnectedClientInfo[numberOfPeerGlo - 1].IP, 
        saveConnectedClientInfo[numberOfPeerGlo - 1].savedPortNo);
        
        /*Why saveConnectedClientInfo[numberOfPeerGlo - 1]: Theo như giá trị khởi tạo ban đầu sẽ là numberOfPeerGlo = 0.
        Sau accept system call, lời gọi hàm được thực thi và numberOfPeerGlo = 1. Server khi kết nối lần đầu tiên sẽ có 
        saveConnectedClientInfo[0].savedPortNo mới đúng nên cần trừ đi 1*/
        
        saveConnectedClientInfo[numberOfPeerGlo-1].savedFileDes = newSocketFD; 

  
        /*tạo một thread chờ để nhận tin nhắn */
        pthreadCreatClientVal = pthread_create (&clientThreadID, NULL, &receiveMessage, NULL );//&saveConnectedClientInfo[numberOfPeerGlo].savedFileDes
    }
}

/**
 * @Description Creating client, lưu trữ ID, IP, port no vào buffer
 * @param argv: cmd từ keyboard trong interface, numberOfPeerPtr: con trỏ chứa địa chỉ lưu giá trị của số-lượng-peer 
 * @return
*/
void createClient(char **argv){ //
    int portNo;
    struct sockaddr_in clientAddr;

    portNo = atoi(argv[2]);

    memset (&clientAddr, '0', sizeof(struct sockaddr_in));

    saveConnectedClientInfo[numberOfPeerGlo].savedFileDes = socket (AF_INET, SOCK_STREAM, 0);
    if ( saveConnectedClientInfo[numberOfPeerGlo].savedFileDes == -1){
        handle_error("socket() in client");
    }

    //pass địa chỉ cho client
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(portNo);                // Port number được chuyển sang network byte order 
    clientAddr.sin_addr.s_addr = inet_addr(argv[1]);    //IP address: translate an Internet address into network byte order

    socklen_t addrLen = sizeof(clientAddr);

    if (connect (saveConnectedClientInfo[numberOfPeerGlo].savedFileDes, (struct sockaddr *)&clientAddr, addrLen) == -1){
        handle_error("connect()");
    }

    printf("connected with server IP: %s, through port no: %d\n", argv[1], portNo);

    //save client connected into buffer
    saveConnections( clientAddr ); //, saveConnectedClientInfo[numberOfPeerGlo].socketFD
    
    /*Why this programming should save connects both server and client:
    - Sau khi option4 thực hiện, nếu chỉ lưu kết nối ở createClient func thì khi sử dụng option5 thì khi liệt kê các port, IP ra sẽ bị thiếu
    -Ex: Chat1-port2222, Chat2-port3333. Chat1 thực hiện connect với chat2 qua port 3333
    Nếu không lưu địa chỉ kết nối ở server, khi list ra các port đang sử dụng:
        Chat1 có: port 3333
        Chat2 có: port: None (Đúng ra phải là Hiển thị port 2222 trong list)
    */
    //printf("port no saved after saving: %d\n", saveConnectedClientInfo[numberOfPeerGlo-1].savedPortNo);       for debugging only
}

/** @Description: Hàm liệt kê các kết nối tới chat-app
 *  @param numberofPeer: số lượng kết nối
 *  @return none 
*/  
void listConnections(int numberOfPeer){
    int i = 0;

    printf("\n-------------------------------------------------------------------\n");
    printf("id: \t  IP address \t\t Port No.\n");
    
    if (numberOfPeer == 1 && saveConnectedClientInfo[i].IP[2] != ' '){  //this statement for one connection only. Nếu chỉ có 1 kết nối numberOfPeerGlo = 1.
        printf(" %d\t| %s \t| %d\n", saveConnectedClientInfo[i].ID, saveConnectedClientInfo[i].IP, 
        saveConnectedClientInfo[i].savedPortNo);
        printf("-------------------------------------------------------------------\n");
        return;
    }
                                                                        //Why: saveConnectedClientInfo[i].IP[2] != ' ' in both if statement, see terminateConnection func for detail
    for ( i; i < numberOfPeer; i++ ){                                   //Nếu có 2 kết nối numberOfPeerGlo = 2 mà ID trong bảng sẽ là 0 và 1 nên i < numberOfPeer
        if (saveConnectedClientInfo[i].IP[2] != ' '){
        printf(" %d\t| %s \t| %d\n", saveConnectedClientInfo[i].ID, saveConnectedClientInfo[i].IP, saveConnectedClientInfo[i].savedPortNo);
        }
    }
    printf("-------------------------------------------------------------------\n");
}

/** @Description: Hàm chấm dứt kết nối có trong danh sách các kết nối
 *  @param argv: đối số từ command line, đây là chuỗi có giá trị là ID muốn chấm dứt kết nối
 *  @return none 
*/ 
void terminateConnection(char **argv){
    char buffer[10];// chuỗi lưu ID
    int terminateNum, portTer, FD;
    
    strncpy(buffer, argv[1], 2);                //copy ID dạng chuỗi vào buffer
    terminateNum = atoi(buffer);                // convert sang int

    if (terminateNum < 0 || terminateNum >= numberOfPeerGlo){
        printf("Invalid Connection!\n");
        return;
    }
    //printf("ID: %d\n", terminateNum);         //for debugging only

    portTer = saveConnectedClientInfo[terminateNum].savedPortNo;
    printf("The connection through port %d terminated.\n", portTer);

    if (close( saveConnectedClientInfo[terminateNum].savedFileDes ) == -1) handle_error("close()");
    
    /* ----------------------------------------------UPGRADE HERE----------------------------------------------
    - Delete data: hiện tại chỉ xóa thông tin lưu ở mảng struct saveConnectedClientInfo chứ chưa sắp xếp lại thứ tự.
    - Sau khi close thì không thể communicate được nữa, vấn đề là chỉ cần xóa data. Tại sao saveConnectedClientInfo[terminateNum].ID
    và saveConnectedClientInfo[terminateNum].savedPortNo không cần thiết xóa, bởi vì chỉ cần thêm điều kiện ở listConnections func,
    IP khác chuỗi-rỗng (đúng ra là chuỗi toàn khoảng trắng) là có thể không in ra data vừa xóa được */
    
    // saveConnectedClientInfo[terminateNum].ID = '\0'; 
    // saveConnectedClientInfo[terminateNum].savedPortNo = '\0';
    memset (saveConnectedClientInfo[terminateNum].IP, ' ', INET_ADDRSTRLEN);
}

int main (int argc, char *argv[])
{
    int optionValue;
    char cmd_argument[CMD_LINE_SIZE];
    char *args[CMD_LINE_SIZE];
    
    numberOfPeerGlo = 0;

    system("clear");
    displayCommandOptions();
    
    //creating server to wait connections
    pthreadCreatSerVal = pthread_create (&serverThreadID, NULL, &createServer, (void *)argv);
    if ( pthreadCreatSerVal != 0) {
        handle_error("pthread_create server");
    }

    while(1){
    fgets( cmd_argument, CMD_LINE_SIZE , stdin );
    cmd_argument[strcspn(cmd_argument, "\n")] = '\0';

    parseCommand (cmd_argument, args);
    
    optionValue = choosingOption(args);
    switch ( optionValue ){
        case 1:                 /*Manual command*/
            printf("Read description above step by step!\n");
            break;

        case 2:                 /*IP-address of this process*/
            getIPAddress();
            break;

        case 3:                 /*Port is listening for incoming connection*/
            if (argc < 2){
                printf("No port provided!\n");
                exit(EXIT_FAILURE);
            }
            getPortNo(argv);
            break;

        case 4:                 /*Establishing a new TCP connection*/
            //Identifying port and IP to avoid self-connection and connect to wrong adress
            if (strcmp(args[2], argv[1]) == 0){
                printf("Invalid port!\n");
                break;
            }
            if (strcmp(args[1], MY_IP) != 0){
                printf("Invalid IP!\n");
                break;
            }
            
            createClient(args); 
            break;

        case 5:                 /*List of all connections*/
            // printf("số lượng kết nối: %d", numberOfPeerGlo); //for debugging only
            listConnections(numberOfPeerGlo);
            break;

        case 6:                 /*Terminating the specific connection*/
            terminateConnection(args);
            break;

        case 7:                 /*Sending message to another process*/
            printf("Starting send.\n");
            sendMessage(args);
            break;           

        case 8:                 /*Close all connections and terminate this process*/
            printf("Program is exiting ...\n");
            exit(EXIT_SUCCESS);
        
        default:
            printf("Invalid command!\n");
    }
    }
    return 0;
}