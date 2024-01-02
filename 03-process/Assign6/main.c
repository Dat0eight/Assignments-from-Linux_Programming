#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char const *argv[])  
{
    pid_t child_pid;                
    int counter = 2, wstatus, returnValue;               

    printf("Gia tri khoi tao cua counter: %d\n", counter);

    child_pid = fork();         
    if (child_pid >= 0) {
        if (0 == child_pid) {       /* Process con */
            printf("\nIm the child process, counter: %d\n", ++counter);
            printf("My PID is: %d, my parent PID is: %d\n", getpid(), getppid());
            while (1);
            
        } else {                    /* Process cha */
            
            returnValue = waitpid(child_pid, &wstatus, 0);
            if(returnValue == -1){ //check waitpid action
                printf("waitpid action unsucceeded!");
                exit(-1);
            } else {
                printf("\nIm the parent process, counter: %d\n", ++counter);
                printf("My PID is: %d\n", getpid());

                //inform status of children process termination
                if (WIFEXITED(wstatus)){
                    printf("the child terminated normally, status: %d", WEXITSTATUS(wstatus));
                } 
                else if(WIFSIGNALED(wstatus)){
                    printf("the child process was terminated by a signal, signal number: %d", WTERMSIG(wstatus));
                }
            }
        }
    } else {
        printf("fork() unsuccessfully\n");      // fork() return -1 nếu lỗi.
    }

    return 0;
}