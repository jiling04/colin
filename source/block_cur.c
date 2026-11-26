#include<copy.h>

//任务量计算模块
int block_cur(const char* srcfile, int pronum)
{
	int fd = open("srcfile",O_RDWR);
	int filesize = lseek(fd,0,SEEK_END);
	int blocksize = 0;
	if(filesize % pronum == 0)
	{
		blocksize = filesize / pronum;
	}
	else
	{
		blocksize = (filesize / pronum) + 1;
	}
	return blocksize;
}
