#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>           
#include <sys/stat.h>       
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define handle_error(msg) do {perror( msg ); exit( EXIT_FAILURE );} while (0)

#define MQ_NAME "/student_mq"
#define MAX_MSG_SIZE 8192 

typedef struct {
    long int priority;
    struct {
        char fullName[50];
        int age;
        char homeLand[100];
        char phoneNumber[20];
    } info;
} inforStudent;

/*Display Menu function*/
void menuFunc(){
    printf("\n\n-------Display Menu-------\n\n");
    printf("1. Input the student information from keyboard.\n");
    printf("2. Get a student information through ID.\n");
    printf("3. Show all the student informations list.\n");
    printf("4. Using ID to delete a student information from list.\n");
    printf("Press 'x' to QUIT.\n");
    printf("NOTE: The  ID number of each student is unique and greater than 0.\n\n");
}

/*Getting information function*/
void getInfo(inforStudent *buff, mqd_t fileDescriptor){
    
    printf("Student ID:");
    scanf("%li", &(buff->priority));
    getchar();

    printf("Student name:");
    fgets(buff->info.fullName, sizeof(buff->info.fullName), stdin);
    buff->info.fullName[strcspn(buff->info.fullName, "\n")] = '\0';

    printf("Age:");
    scanf("%d", &(buff->info.age));
    getchar();

    printf("Home land:");
    fgets(buff->info.homeLand, sizeof(buff->info.homeLand), stdin);
    buff->info.homeLand[strcspn(buff->info.homeLand, "\n")] = '\0'; 

    printf("Phone number:");
    fgets(buff->info.phoneNumber, sizeof(buff->info.phoneNumber), stdin);
    buff->info.phoneNumber[strcspn(buff->info.phoneNumber, "\n")] = '\0';

    //check data from keyboard that stored yet
    printf("Data stored include: ID:%li - name: %s - age: %d - home land: %s - phone: %s\n",
    buff->priority, buff->info.fullName, buff->info.age, buff->info.homeLand, buff->info.phoneNumber);

    /*POSIX message queue: send data*/

    // if (mq_getattr(fileDescriptor, &messageAttr) == -1){         >>>Just check Max size message
    //     handle_error("mq_getattr()");
    // }

    // printf("Max message size: %ld\n", messageAttr.mq_msgsize);

    if( mq_send(fileDescriptor, (const char*)buff, sizeof(buff) , (unsigned int)buff->priority ) == -1){ 
        handle_error("mp_send()");
    }
    /*Ở đây mình đang muốn set msg_prio là priority của struct inforStudent
    Do vậy khi mình sử dụng mq_receive() ở Chức năng hiển thị thông tin sinh viên thông qua ID, 
    đối số msg_prio sẽ là con trỏ trỏ tới giá trị priority 
    Mình hiểu priority như type message, các process có thể dựa vào nó để lấy ra tin nhắn phù hợp*/
}

int main()
{
    char option;
    mqd_t fileDescriptor;
    inforStudent person, IDperson;
    inforStudent  *buff, *ID_buff;
    
    menuFunc();
    
    fileDescriptor = mq_open( MQ_NAME, O_CREAT | O_RDWR | O_APPEND, 0666 , 0);
    if ( fileDescriptor == -1 ){
        handle_error("mq_open()");
    }
    
    while(1)
    {
        printf("Your option: \n");
        scanf("%c", &option);

        switch (option)
        {
            case '1': //Getting information and send data into message queue
                buff = &person;
                
                printf("Input the student information from keyboard\n"); 
                getInfo( buff , fileDescriptor );

                break;

            case '2': //Get a student information through ID
                ID_buff = &IDperson;

                printf("Get a student information through ID.\n");
                printf("Input ID student who you want to get information:");
                scanf("%u",(unsigned int *)&ID_buff->priority);
                
                if ( mq_receive(fileDescriptor, (char*)buff, 10000 , (unsigned int *)&ID_buff->priority ) == -1 ){ // /  - 0 sizeof(buff) 
                    handle_error("mq_receive()");
                }
                
                printf("ID:%li - name: %s - age: %d - home land: %s - phone: %s\n",
                buff->priority, buff->info.fullName, buff->info.age, buff->info.homeLand, buff->info.phoneNumber);
                break;

            case '3':
                printf("option3");
                break;

            case '4':
                printf("option4");
                break;
            
            case 'x':
                exit(EXIT_SUCCESS);

            default: 
                printf("Not matched!. Try another options.");
                break;

        }
    }
    return 0;

}