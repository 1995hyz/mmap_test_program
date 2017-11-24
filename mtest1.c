#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void handlerSegV();
void handlerBus();
void handlerSys();
void handlerAbrt();
void handlerIll();
void runHandler();

int fd=0;
size_t length=4096;
off_t offset=0;
char *addr;
char buf[2]={'a','\0'};

void runHandler(){
	signal(SIGSEGV,handlerSegV);
	signal(SIGBUS,handlerBus);
	signal(SIGSYS,handlerSys);
	signal(SIGABRT,handlerAbrt);
	signal(SIGILL,handlerIll);
}

void handlerSegV(){
	fprintf(stderr,"Signal SIGSEGV Received\n");
	exit(11);
}

void handlerBus(){
	fprintf(stderr,"Signal SIGBUS Received\n");
	exit(10);
}

void handlerSys(){
	fprintf(stderr,"Signal SIGSYS Received\n");
	exit(12);		
}

void handlerAbrt(){
	fprintf(stderr,"Signal SIGABRT Received\n");
	exit(6);
}

void handlerIll(){
	fprintf(stderr,"Signal SIGILL Received\n");
	exit(4);
}

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
	runHandler();
	memcpy(addr,buf,strlen(buf)+1);
	//write(STDOUT_FILENO,addr,1);
	//printf("\n");
}
