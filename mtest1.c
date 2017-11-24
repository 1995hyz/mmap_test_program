#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int fd=0;
size_t length=4096;
off_t offset=0;
char *addr;
char buf[2]={'a','\0'};

void main(int argc, char* argv[]){
	if(!(fd=open("./mtext.txt",O_RDONLY))){
		fprintf(stderr,"Open File mtext.txt Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	errno=0;
	addr=mmap(NULL,length,PROT_READ,MAP_PRIVATE,fd,offset);
	if(errno!=0){
		fprintf(stderr,"Map Region Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	memcpy(addr,buf,strlen(buf)+1);
	//write(STDOUT_FILENO,addr,1);
	//printf("\n");
}
