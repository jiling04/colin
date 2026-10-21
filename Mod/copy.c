#include<copy.h>

int main(int argc, char** argv)
{
	const char* srcfile = argv[1];
	const char* destfile = argv[2];
	int offset = atoi(argv[3]);
	int blocksize = atoi(argv[4]);
	int srcfd,destfd;
	srcfd = open(srcfile,O_RDONLY);
	destfd = open(destfile,O_WRONLY|O_CREAT , 0664);
	lseek(srcfd, offset, SEEK_SET);
	lseek(destfd , offset, SEEK_SET);
	//从源文件中读取blocksize数据
	char buf[4096];
	int total_read = 0;
	int n;
	while(total_read < blocksize)
	{
		int to_read = blocksize - total_read;
		if(to_read > sizeof(buf))
		{
			to_read = sizeof(buf);
		}
		n = read(srcfd, buf, to_read);
		if(n <= 0) break;
		write(destfd, buf, n);
		total_read += n;
	}
	close(srcfd);
	close(destfd);
	return 0;
}
