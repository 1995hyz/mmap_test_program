#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

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
	addr=mmap(NULL,length*2,PROT_READ,MAP_PRIVATE,fd1,offset);
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
	memcpy(addr,buf,strlen(buf));
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

void mtest4(int b){
	int fd4=0;
	char buf3[10]={'0','1','2','3','4','5','6','7','8','9'};
	char buf4[4102];
	struct stat testFile;
	int originalSize=0;
	int changedSize=0;
	if(!(fd4=open("./mtext.txt",O_RDWR|O_CREAT|O_TRUNC,0666))){
		fprintf(stderr,"Open/Create mtext.txt Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	int i=0;
	for(i;i<410;i++){ 
		strncat(buf4,buf3,10);
	}
	write(fd4,buf4,strlen(buf4));
	if(fstat(fd4,&testFile)<0){
		fprintf(stderr,"Uable to stat mtext.txt, Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	originalSize=(int)testFile.st_size;	
	errno=0;
	addr=mmap(NULL,8192,PROT_READ|PROT_WRITE,MAP_SHARED,fd4,offset);
	if(errno!=0){
		fprintf(stderr,"Map Region Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);	
	}
	strcat(buf4,"*");
	memcpy(addr,buf4,strlen(buf4));
	if(b==4){
		if(fstat(fd4,&testFile)<0){
			fprintf(stderr,"Uable to stat mtext.txt, Error: %s\n",strerror(errno));
			exit(EXIT_FAILURE);
		}
		changedSize=(int)testFile.st_size;
		if(changedSize=originalSize){
			exit(1);
		}
		else{
			exit(0);
		}
	}
	if(b==5){
		char bufComp[2]={'\0','\0'};
		int changedSizePro5=0;
		offset=16;
		lseek(fd4,offset,SEEK_END);
		write(fd4,"E",sizeof(char));
		offset=4100;
		lseek(fd4,offset,SEEK_SET);
		read(fd4,bufComp,sizeof(char));
		if(strncmp(bufComp,"*",sizeof(char))==0){
			exit(0);
		}
		else{
			exit(1);
		}
	}
}	
		
void main(int argc, char *argv[]){
	char InitBuf[16];
	strcpy(InitBuf,"For Memory Testing\0");
	if(!(fd=open("./mtext.txt",O_RDWR|O_CREAT|O_TRUNC,0666))){
		fprintf(stderr,"Open/Create mtext.txt Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	write(fd,InitBuf,strlen(InitBuf));
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
		case 4:
			mtest4(4);
			break;
		case 5:
			mtest4(5);
			break;
		default:
			exit(1);
	}
}	
	
