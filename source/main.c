#include<copy.h>

//任务量计算模块
int block_cur(const char* srcfile, int pronum);
//多进程的创建以及子进程的拷贝
int process_create(const char* srcfile,const char* destfile, int pronum,int blocksize);
//父进程阻塞回收
int process_wait();
//参数验证模块
int check_pram(int argc,const char* srcfile,int pronum);

int main(int argc, char** argv)
{
	const char* srcfile = argv[1];
	const char* destfile = argv[2];
	int pronum = atoi(argv[3]);
	int srcfd = open(srcfile,O_RDONLY);
	int destfd = open(destfile,O_WRONLY|O_CREAT , 0664);
	int len;
	int blocksize = block_cur(srcfile, pronum);
	char* buffer[blocksize];
	bzero(buffer,sizeof(buffer));
	len = read(srcfd, buffer ,sizeof(buffer));
	write(destfd,buffer,len);
	process_create(srcfile, destfile, pronum, blocksize);
	process_wait();
	printf("pid finished\n");
	return 0;
}

