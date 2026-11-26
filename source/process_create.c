#include<copy.h>

int process_create(const char* srcfile, const char* destfile, int pronum, int blocksize)
{
	//创建子进程
	int i;
	pid_t pid;
	for(i = 0;i < pronum;i++)
	{
		pid = fork();
		if(pid < 0)
		{
			perror("fork failed");
			exit(1);
		}
		else if(pid == 0)
		{
			int offset = i * blocksize;
			char str_offset[10];
			char str_blocksize[10];
			bzero(str_offset,10);
			bzero(str_blocksize,10);
			sprintf(str_offset,"%d",offset);
			sprintf(str_blocksize,"%d",blocksize);
			printf("child pid %d offset %d\n",i,getpid());
			execl("/home/colin/20251018/process_copy/source/COPY","COPY",srcfile,destfile,str_offset,str_blocksize,NULL);
			//如果重载失败
			perror("execl failed");
			exit(1);
		}
	}
	return 0;
}
