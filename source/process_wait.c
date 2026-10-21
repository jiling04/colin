#include<copy.h>

int process_wait(void)
{
	pid_t pid;
	int status;
	while((pid = wait(&status)) > 0)
	{
		printf("child process finished pid %d status %d\n",pid, WEXITSTATUS(status));
	}
	return 0;
}
