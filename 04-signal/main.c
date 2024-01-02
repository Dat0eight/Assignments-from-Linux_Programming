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

void sig_handler3(int num)
{
    printf("Im signal handler3: %d\n", num);
    printf("Im the message from SIGUSR2 signal");
	// exit(EXIT_SUCCESS);
}
 
int main()
{
  	if (signal(SIGINT, sig_handler1) == SIG_ERR) {
		fprintf(stderr, "Cannot handle SIGINT\n");
		// exit(EXIT_FAILURE);
	} else {
        printf ("\nChao Dat!\n");
    }

	signal(SIGUSR1, sig_handler2);
	signal(SIGUSR2, sig_handler3);
 	// sleep(4);
	// kill(getpid(), SIGINT);

	// printf("process ID: %d\n", getpid());
	while (1)
	{
		// do nothing.
	 	// printf("hello\n");
		// sleep(2);
	}

}