  #include <sys/mman.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <regex.h>
  
  void regexMatch(int regnum,char* mmap_ptr,int size)
  {
      char* regstr = "<a[^>]*href=\"\\([^\"]*\\)\"[^>]*>\\([^<]*\\)</a>";
      regmatch_t match[regnum];
      regex_t reg;
      regcomp(&reg,regstr,0);
      char url[1024];
      char title[1024];
      while(regexec(&reg,mmap_ptr,regnum,match,0)==0)
      {
          bzero(url,sizeof(url));
          bzero(title,sizeof(title));
          snprintf(url,match[1].rm_eo - match[1].rm_so + 1,"%s",mmap_ptr + match[1].rm_so);
          snprintf(title,match[2].rm_eo - match[2].rm_so + 1,"%s",mmap_ptr + match[2].rm_so);
          mmap_ptr += match[0].rm_eo;
          printf("%s\t%s\n",url,title);
      }
      regfree(&reg);
  
  }
  
  
  int main(void)
  {
      //1.打开映射文件
      int fd = open("url.txt",O_RDONLY);
      //2.获取文件大小
      int fsize = lseek(fd,0,SEEK_END);
      //3.文件映射
      char* mmap_ptr = NULL;
      char* copy_ptr = NULL;
      mmap_ptr = mmap(NULL,fsize,PROT_READ|PROT_WRITE,MAP_PRIVATE,fd,0);
      copy_ptr = mmap_ptr;
      regexMatch(3,mmap_ptr,fsize);
      munmap(copy_ptr,fsize);
      close(fd);
      printf("done.\n");
      return 0;
  }
  

