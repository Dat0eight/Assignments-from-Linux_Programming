#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void prevent_Zombie(int signum){
    printf("This is SIGCHLD signal!\n");
    wait(NULL);
}

int main (int argc, char const *argv[])  
{
    pid_t child_pid;                
    int wstatus, returnValue;               

    child_pid = fork();         
    if (child_pid >= 0) {
        if (0 == child_pid) {       /* Process con */
            printf("\nIm the child process\n");
            printf("My PID is: %d, my parent PID is: %d\n", getpid(), getppid());
            
            while(1);
            
            
        } else {                    /* Process cha */
            // wait(&wstatus); >>> this is one of the ways to prevent zombie process
            signal(SIGCHLD , prevent_Zombie);
            printf("\nIm the parent process\n");
            printf("My PID is: %d\n", getpid());
            while(1);
            }
     } else {
        printf("fork() unsuccessfully\n");      // fork() return -1 nếu lỗi.
        }

    return 0;
}