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
    char fullName[50];
    int age;
    char homeLand[100];
    char phoneNumber[20];
} info_struct;

typedef struct {
    long int priority;
    info_struct info; 
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
void getInfo( inforStudent *buff, mqd_t fileDescriptor ){ 
    
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

    //check data from keyboard that stored in buff yet
    printf("Data stored include: ID:%li - name: %s - age: %d - home land: %s - phone: %s\n",
    buff->priority, buff->info.fullName, buff->info.age, buff->info.homeLand, buff->info.phoneNumber);

    /*POSIX message queue: send data*/
    if( mq_send( fileDescriptor, (const char *)buff, sizeof(inforStudent) , 0 ) == -1){ // đã sai ở chỗ sizeof(buff) 
        handle_error("mp_send()");
    } else printf("Send message into the queue successfully.\n");
}

int main()
{
    mqd_t fileDescriptor;
    inforStudent person, getInfoVar; //, IDperson
    inforStudent  *buff, *getInfoBuff; //, *ID_buff
    char option;
    struct mq_attr messageAttr;
    int checkAvailable = 0;
    unsigned int ID_buff;

    fileDescriptor = mq_open( MQ_NAME, O_CREAT | O_RDWR | O_NONBLOCK , 0666 , 0);
    if ( fileDescriptor == -1 ){
        handle_error("mq_open()");
    }  
    
    if (mq_getattr(fileDescriptor, &messageAttr) == -1){  // in order to get attributes of mq       
    handle_error("mq_getattr()");
    }

    menuFunc();

    while(1)
    {
        printf("Your option:\n");
        scanf(" %c", &option);
        switch (option)
        {
            case '1': //Getting information and send data into message queue
                buff = &person;
                
                printf("Input the student information from keyboard\n"); 
                getInfo( buff , fileDescriptor );

                break;

            case '2': //Get a student information through ID
                getInfoBuff = &getInfoVar;

                printf("Get a student information through ID.\n");
                printf("Input ID student who you want to get information:");
                scanf("%u", &ID_buff);
                
                for (int i = 0; i < messageAttr.mq_curmsgs ; i++) { //nhận message về kiểm tra: nếu oke thì in ra  và send lên lại. Nếu k ok thì cũng send lên lại được
                    if ( mq_receive(fileDescriptor, (char*)getInfoBuff, MAX_MSG_SIZE , 0 ) == -1 ){ /*  - 0 sizeof(buff) -  (unsigned int *)&ID_buff->priority */
                        handle_error("mq_receive()");
                    }  
                    
                    if ( getInfoBuff->priority == ID_buff ){
                        printf("ID:%li - name: %s - age: %d - home land: %s - phone: %s\n",
                        getInfoBuff->priority, getInfoBuff->info.fullName, getInfoBuff->info.age, getInfoBuff->info.homeLand, getInfoBuff->info.phoneNumber);
                        
                        checkAvailable = 1;
                        
                        if( mq_send(fileDescriptor, (const char *)getInfoBuff, sizeof(inforStudent) , 0 ) == -1){ //
                            handle_error("mp_send()");
                        } 
                        break;
                    } 
                    if( mq_send(fileDescriptor, (const char *)getInfoBuff, sizeof(inforStudent) , 0 ) == -1){ //
                    handle_error("mp_send()");
                    }
                }

                if (checkAvailable == 0) {printf ("There is not a student with this ID\n");};
                checkAvailable = 0;
                break;

            case '3':
                printf("Show the student list\n");

                //Check the number of messages currently held in the queue
                if (messageAttr.mq_curmsgs == 0){
                    printf("The queue is empty!. Choosing Option 1 to input student information.\n");
                    break;

                } else {
                    for (int i = 0; i < messageAttr.mq_curmsgs ; i++){ //if the queue is not empty, show student list
                        if ( mq_receive(fileDescriptor, (char*)getInfoBuff, MAX_MSG_SIZE , 0) == -1 ){ 
                            handle_error("mq_receive()");
                        }
                        
                        printf("ID:%li - name: %s - age: %d - home land: %s - phone: %s\n",
                        getInfoBuff->priority, getInfoBuff->info.fullName, getInfoBuff->info.age, getInfoBuff->info.homeLand, getInfoBuff->info.phoneNumber);
                        
                        if( mq_send(fileDescriptor, (const char *)getInfoBuff, sizeof(inforStudent) , 0 ) == -1){ //
                            handle_error("mp_send()");
                        }
                    }
                    break;
                }

            case '4':
                getInfoBuff = &getInfoVar;

                printf("Delete a student information through ID.\n");
                printf("Input ID student who you want to delete information:");
                scanf("%u", &ID_buff);
                
                for (int i = 0; i < messageAttr.mq_curmsgs ; i++) { //nhận message về kiểm tra: nếu oke thì in ra  và send lên lại. Nếu k ok thì cũng send lên lại được
                    if ( mq_receive(fileDescriptor, (char*)getInfoBuff, MAX_MSG_SIZE , 0 ) == -1 ){ /*  - 0 sizeof(buff) -  (unsigned int *)&ID_buff->priority */
                        handle_error("mq_receive()");
                    }  
                    
                    if ( getInfoBuff->priority == ID_buff ){
                        printf("The information ID:%li - name: %s - age: %d - home land: %s - phone: %s was deleted!\n",
                        getInfoBuff->priority, getInfoBuff->info.fullName, getInfoBuff->info.age, getInfoBuff->info.homeLand, getInfoBuff->info.phoneNumber);
        
                        checkAvailable = 1;
                        messageAttr.mq_curmsgs = messageAttr.mq_curmsgs - 1;
                        break;
                        } 
             
                    if( mq_send(fileDescriptor, (const char *)getInfoBuff, sizeof(inforStudent) , 0 ) == -1){ //
                    handle_error("mp_send()");
                    }
                }

                if (checkAvailable == 0) {printf ("There is not a student with this ID\n");};
                checkAvailable = 0;
                break;
            case '5': //just check the number of message in queue only
                printf("Number of messages currently in queue: %ld\n", messageAttr.mq_curmsgs);
                break;

            case 'x':
                mq_close(fileDescriptor);
                if (mq_unlink(MQ_NAME) == -1){
                    handle_error("mq_unlink()");
                }
                exit(EXIT_SUCCESS);

            default: 
                printf("Not matched!. Try another options.\n");
                break;

        }
    }
    return 0;
}