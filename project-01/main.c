#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>


#define ARG_BUFFER_SIZE 64
#define CMD_LINE_SIZE 1024
#define handle_error(msg) do { perror( msg ); exit ( EXIT_FAILURE ); } while(0);

/**
* @description: this function will display prompt
* @param None
* @return None
*/
void menu_display_func(){
    printf("\n\n#####Project1. Terminal - Simple Shell#####\n");
    printf("Input command-line below:\n");
}

/**
* @description: This function will split command-line into argv array.
* @param cmd_argument is the string that input from shell
* @return None
*/
void parse_command(char *cmd_argument, char **argv){
    int i = 0;

    //khởi tạo argv là NULL
    while (i < ARG_BUFFER_SIZE){
        argv[i] = NULL;
        i++;
    }
    
    //tách command-line lưu vào argv
    i = 0;
    argv[i] = strtok(cmd_argument, " ");
    while (argv[i] != NULL){
        i++;
        argv[i] = strtok(NULL, " ");
    }
}

/**
* @description: this function will compare options.
* @param cmd_argument is the string that input from shell
* @return   If the command-line is the same as execute option, return 1.
            If the command-line is the same as output rediction option, return 2.
            If the command-line is the same as pipe option, return 3.
            If the command-line is the same as quit option , return 6.
            If the command-line was not found, return -1.
*/
int compare_argument_func(char **argv){
    int i = 0;
    int check_option_value = 1;
    
    while(argv[i] != NULL){
        //Kiểm tra nếu đúng là quit option thì return liền
        if (strcmp( argv[i] , "exit") == 0 && argv[i+1] == NULL){
        return check_option_value = 6;
        }
        if (strcmp( argv[i] , "quit") == 0 && argv[i+1] == NULL){
        return check_option_value = 6;
        }

        //Lần lượt kiểm tra các argv để xem có phải là output rediction option  không. Ở đây chỉ mới hoàn tất được xác định '>' redirection. Còn khác, báo lỗi.HERE
        if (strcmp( argv[i] , ">") == 0 && strcmp( argv[i+1] , ">") != 0){
            return check_option_value = 2;
        }
        if (strcmp( argv[i] , ">") == 0 && strcmp( argv[i+1] , ">") == 0){ // Nếu có xuất hiện "> >" ->loại
            return check_option_value = -1;
        }  
        if (strcmp( argv[i] , "<") == 0 || strcmp( argv[i] , "<<") == 0 || strcmp( argv[i] , ">>") == 0){ // Nếu có xuất hiện '<' "<<" ">>" -> loại
            return check_option_value = -1;
        }  
        
        //Lần lượt kiểm tra các argv để xem có phải là pipe option  không. 
        if (strcmp( argv[i] , "|") == 0 && strcmp( argv[i+1] , "|") != 0){ //Loại bỏ trường hợp "| |"
            return check_option_value = 3;
        }   
        if (strcmp( argv[i] , "||") == 0 ){ // Nếu có xuất hiện "||" ->loại
            return check_option_value = -1;
        }
        i++;                  
    }
    return check_option_value;
}
/**
 * @description: Hàm xác định vị trí của kí tự '>' trong command-line
 * @param command-line parsed
 * @return vị trí '>' trong line
*/
int identify_position_redirect(char **argv){
    int i = 0, position_value = 0;
    
    while(argv[i] != NULL){
        if (strcmp( argv[i] , ">") == 0 ){
            return position_value = i;
        }
        i++;        
    }
    return position_value;
}

/**
 * @description: Hàm xác định vị trí của kí tự '|' trong command-line
 * @param command-line parsed
 * @return vị trí '|' trong line
*/
int identify_position_pipe(char **argv){
    int i = 0, position_value = 0;
    
    while(argv[i] != NULL){
        if (strcmp( argv[i] , "|") == 0 ){
            return position_value = i;
        }
        i++;        
    }
    return position_value;
}

/**
 * @description: thực thi executable files
 * @param command-line parsed
 * @return None
*/
void execvp_func(char **argv){
    char *cmd = argv[0];
    char *const cmd_list[] = {argv[0], argv[1], NULL};
    if (execvp(cmd , cmd_list) < 0){
        handle_error("execvp()");
    }
    printf("Command not found!\n"); //nếu process thực thi được thì không in dòng này
}

/**
 * @description: hàm này set param cho execvp và thực thi. Hàm nãy hữu ích khi thao tác với lệnh ls -l > <filename>
 * @param argv: command-line parsed. redirect_position_index: vị trí '>' trong command-line
 * @return None
*/
void parse_execvp_redict(char **argv, int redirect_position_index){
    int i = 0;
    char *cmd = argv[i];
    char *cmd_list[ARG_BUFFER_SIZE];
    
    while (i < redirect_position_index){
        cmd_list[i] = argv[i];
        i++;
    }
    cmd_list[i] = NULL;

    if (execvp(cmd , cmd_list) < 0){
        handle_error("execvp()");
    }
    printf("Command not found!\n"); //nếu process thực thi được thì không in dòng này
}

/**
 * @description: hàm có tác dụng ngăn tình trạng hình thành zoombie process
 * @param signal number
 * @return None
*/
void handler_sigchld(int signum){
    wait(NULL);
}

/****************************************************************************************
 * 
 *          ***
 *          ***
 *          ********
 *          ********
 *          
 * 
 * ***************************************************************************************
*/
/**
 * @description: xử lý command-line chứa '|'. Ở đây còn tồn tại 1 vấn đề đó là sau khi 
 * thực hiện command từ $myshell xong thì quit chương trình luôn. NHỜ CAO NHÂN PHẢN HỒI GIÚP Ạ!!!
 * @param signal command-line parsed
 * @return None
*/
void parse_execvp_pipe( char **argv ){
    int i = 0;
    int pipe_position_index = identify_position_pipe(argv);
    /**Ex: ls -l | sort -r -> "ls", "-l" are the first cmds, "sort", "-r" are the second cmds. argv[end] = NULL.
    *   In this ex, pipe_position_index = 2*/
    char *first_cmd = argv[i], *second_cmd = argv[pipe_position_index + 1];
    char *first_cmd_list[ARG_BUFFER_SIZE], *second_cmd_list[ARG_BUFFER_SIZE];
    int pipe_fd[2];
    pid_t child_pid;
    
    for ( i; i < pipe_position_index; i++ ){ //pass argv(command entered keyboard) into first_cmd_list
        first_cmd_list[i] = strdup(argv[i]);
    }
    first_cmd_list[i++] = NULL;

    while ( argv[i] != NULL ){ //pass argv(command entered keyboard) into second_cmd_list
        second_cmd_list[i - pipe_position_index - 1] = strdup(argv[i]);
        i++;
    }
    second_cmd_list[i - pipe_position_index - 1] = NULL;

    if ( pipe(pipe_fd) < 0 ) handle_error("pipe()");          //create pipe
    /*--------------NOTE SECTION: I have a problem in this one. Finally, I got it. REMEMBER ME---------------*/
    child_pid = fork();
    if ( child_pid >= 0 ) {
        if ( 0 == child_pid ) {       /*child process đóng vai trò là đầu write*/
            close ( pipe_fd[0] );     /*Đóng đầu read*/
            dup2 ( pipe_fd[1], STDOUT_FILENO ); /*Liên kết đầu ra chuẩn với đầu write của ống*/
                
            if ( execvp(first_cmd , first_cmd_list ) < 0) handle_error("execvp_child()");

        } else {                    /* parent process đóng vai trò là đầu read*/
            signal ( SIGCHLD, handler_sigchld ); /*Ngăn ngừa zoombie process*/
            close(pipe_fd[1]);  /*Đóng đầu write*/
            dup2 ( pipe_fd[0], STDIN_FILENO ); /*Liên kết đầu vào chuẩn với đầu read của ống*/

            if ( execvp(second_cmd , second_cmd_list ) < 0) handle_error("execvp_parent()");
        }
    /*--------------------------------------------------------------------------------------------------------*/
    } else printf("fork() unsuccessfully\n"); 

    printf("Command not found!\n"); //nếu process thực thi được thì không in dòng này
}
/**
 * @description:Hàm thực hiện chuyển hướng output
 * @param argv được tách
 * @return None
*/
void redirect_func(char **argv, int redirect_position_index){
    int redirect_fd;
    redirect_fd = open ( argv[redirect_position_index + 1], O_RDWR | O_APPEND | O_CREAT , 0666);
    if (redirect_fd == -1){
        handle_error("redirect_open()");
    }

    dup2(redirect_fd, STDOUT_FILENO);
    if (close(redirect_fd) == -1){
        handle_error("redirect_close()");
    }
    parse_execvp_redict(argv, redirect_position_index);
}

/**
 * @description:Hàm thực hiện chức năng của case1: thực thi command-line
 * @param child_pid, argv được tách
 * @return None
*/
void option1_func (char **argv){
    pid_t child_pid = fork();
    if (child_pid >= 0) {
        if (0 == child_pid) {       /* Process con */
            execvp_func(argv);

        } else {                    /* Process cha */
            // do nothing
        }
    } else printf("fork() unsuccessfully\n"); 
}

/**
 * @description:Hàm thực hiện chức năng của case1: thực thi output rediction option
 * @param child_pid, argv được tách
 * @return None
*/
void option2_func ( char **argv ){
    int redirect_position_index;
    pid_t child_pid = fork();
    if (child_pid >= 0) {
        if (0 == child_pid) {       /* Process con */
            redirect_position_index = identify_position_redirect(argv);
            redirect_func(argv, redirect_position_index);

        } else {                    /* Process cha */
            // do nothing
        }
    } else printf("fork() unsuccessfully\n"); 
}

void main( int argc, char *argv[] ) 
{
    char cmd_argument[CMD_LINE_SIZE];
    char *args[ARG_BUFFER_SIZE];
    int continue_loop = 1;
    //Hiển thị giao diện
    menu_display_func();

    while(continue_loop)
    {        
        //Nhập thông tin vào
        printf("$myshell\t");
        fgets(cmd_argument, sizeof(cmd_argument), stdin);
        cmd_argument[strcspn(cmd_argument, "\n")] = '\0';

        //Phân tích command-line
        parse_command(cmd_argument, args);

        //So sánh xem option nào phù hợp
        int option_value = compare_argument_func(args);
        switch (option_value) {
        case -1:
            printf("Command was not found!\n");
            break;

        case 1: //basic execute option
            option1_func( args );
            break;

        case 2: //output rediction ">" option
            option2_func( args );
            break;
        
        case 3: //pipe option
            parse_execvp_pipe( args ); 
            break; 
         
        case 6:
            printf("Program is quitting...!\n");
            sleep(2); 
            exit(EXIT_SUCCESS);
                               
        default:
            printf("This is default option!\n");
            exit(EXIT_SUCCESS);
        }
        sleep(1);
    }
}