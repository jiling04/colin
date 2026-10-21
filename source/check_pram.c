#include<copy.h>


int check_pram(int argc,const char* srcfile,int pronum)
{
	if(argc < 4) return -1;
	if((access(srcfile, F_OK))!= 0) return -1;
	if(pronum < 1||pronum >=100)return -1;	
	return 0;
}
