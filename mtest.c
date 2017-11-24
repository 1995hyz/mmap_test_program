#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

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

void mtest1(){
	int fd1=0;
	char buf[2]={'a','\0'};
	if(!(fd1=open("./mtext.txt",O_RDONLY))){
		fprintf(stderr,"Open File mtext.txt Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	errno=0;
	addr=mmap(NULL,length,PROT_READ,MAP_PRIVATE,fd1,offset);
	if(errno!=0){
		fprintf(stderr,"Map Region Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	runHandler();
	memcpy(addr,buf,strlen(buf));
	if(!(memcmp(buf,addr,strlen(buf)))){
		fprintf(stderr,"Write to Memory, Succeed\n");
		exit(EXIT_SUCCESS);
	}
	else{
		fprintf(stderr,"Write to Memory, Failed\n");
		exit(255);
	}	
}

void mtest2(int a){
	int fd2=0;
	char buf[2]={'a','\0'};
	if(!(fd2=open("./mtext.txt",O_RDWR))){
		fprintf(stderr,"Open File mtext.txt Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	errno=0;
	if(a==2){
		addr=mmap(NULL,length,PROT_READ|PROT_WRITE,MAP_SHARED,fd2,offset);
	}
	else{
		addr=mmap(NULL,length,PROT_READ|PROT_WRITE,MAP_PRIVATE,fd2,offset);
	}
	if(errno!=0){
		fprintf(stderr,"Map Region Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	//write(fd2,"a",sizeof (char));
	memcpy(addr,buf,strlen(buf));
	/*if((lseek(fd2,offset,SEEK_SET))<0){		//Reset file offset to 0
		fprintf(stderr,"Reset mtext.txt Offset Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}*/
	read(fd2,buf,sizeof (char));
	if(!(memcmp(buf,"a",sizeof (char)))){
		fprintf(stderr,"Byte Has Changed\n");
		exit(0);
	}
	else{
		fprintf(stderr,"Byte Remains the Same\n");
		exit(1);
	}
}
		
void main(int argc, char *argv[]){
	switch(atoi(argv[1])){
		case 1:
			mtest1();
			break;
		case 2:
			mtest2(2);
			break;
		case 3:
			mtest2(3);
			break;
		default:
			exit(1);
	}
}	
	
