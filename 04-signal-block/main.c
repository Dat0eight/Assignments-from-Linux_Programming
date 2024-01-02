//**************BaiTap9***********************


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
 


void sig_handler1()
{
	printf("\nIm signal handler1: \n" );
    printf("\nIm the message from Ctrl+C action");
	// while (1);
	// exit(EXIT_SUCCESS);
}

void sig_handler2(int num)
{
	printf("Im signal handler2: %d\n", num);
    printf("Im the message from SIGUSR1 signal\n");
}
// code below unlock the SIGINT signal
void sig_handler3(int num)
{
 
    printf("Im signal handler3: %d\n", num);
    printf("Im the message from SIGUSR2 signal\n");
}
 
int main()
{
  	sigset_t old_set , new_set ;

    if (signal(SIGINT, sig_handler1) == SIG_ERR) {
		fprintf(stderr, "Cannot handle SIGINT\n");
		// exit(EXIT_FAILURE);
	} else {
        printf ("\nChao Dat!\n");
    }

	signal(SIGUSR1, sig_handler2);
	signal(SIGUSR2, sig_handler3);

    sigemptyset(&new_set);
	sigemptyset(&old_set);

    sigaddset(&new_set, SIGINT);
	
 	printf("new_set is %x\n", new_set);

        if (sigprocmask(SIG_SETMASK, &new_set, &old_set) == 0) {
	 	// sigprocmask(SIG_SETMASK, NULL, &old_set);
		if (sigismember(&new_set, SIGINT) == 1 ) {
			printf("SIGINT exist\n");
		} else if (sigismember(&new_set, SIGINT) == 0) {
			printf("SIGINT does not exist\n");
		}
       	}
    while(1){
        //do nothing
    }

}