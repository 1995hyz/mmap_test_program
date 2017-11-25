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
	fprintf(stderr,"Signal SIGSEGV(11) Received\n");
	remove("./mtext.txt");
	exit(11);
}

void handlerBus(){
	fprintf(stderr,"Signal SIGBUS(10) Received\n");
	remove("./mtext.txt");
	exit(10);
}

void handlerSys(){
	fprintf(stderr,"Signal SIGSYS(12) Received\n");
	remove("./mtext.txt");
	exit(12);		
}

void handlerAbrt(){
	fprintf(stderr,"Signal SIGABRT(6) Received\n");
	remove("./mtext.txt");
	exit(6);
}

void handlerIll(){
	fprintf(stderr,"Signal SIGILL(4) Received\n");
	remove("./mtext.txt");
	exit(4);
}

void mtest1(){
	int fd1=0;
	char buf[2]={'a','\0'};
	char InitBuf[16];
	strcpy(InitBuf,"For Memory Testing\0");
	if(!(fd1=open("./mtext.txt",O_RDWR|O_CREAT|O_TRUNC,0666))){
		fprintf(stderr,"Open File mtext.txt Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	write(fd1,InitBuf,strlen(InitBuf));
	errno=0;
	addr=mmap(NULL,length*2,PROT_READ,MAP_PRIVATE,fd1,offset);
	if(errno!=0){
		fprintf(stderr,"Map Region Error: %s\n",strerror(errno));
		remove("./mtext.txt");
		exit(EXIT_FAILURE);
	}
	runHandler();
	memcpy(addr,buf,strlen(buf));
	if(!(memcmp(buf,addr,strlen(buf)))){
		fprintf(stderr,"Write to Memory, Succeed\n");
		remove("./mtext.txt");
		exit(EXIT_SUCCESS);
	}
	else{
		fprintf(stderr,"Write to Memory, Failed\n");
		remove("./mtext.txt");
		exit(255);
	}	
}

void mtest2(int a){
	int fd2=0;
	char buf[2]={'a','\0'};
	char InitBuf[16];
	strcpy(InitBuf,"For Memory Testing\0");
	if(!(fd2=open("./mtext.txt",O_RDWR|O_CREAT|O_TRUNC,0666))){
		fprintf(stderr,"Open File mtext.txt Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	write(fd2,InitBuf,strlen(InitBuf));
	errno=0;
	if(a==2){
		addr=mmap(NULL,length,PROT_READ|PROT_WRITE,MAP_SHARED,fd2,offset);
	}
	else{
		addr=mmap(NULL,length,PROT_READ|PROT_WRITE,MAP_PRIVATE,fd2,offset);
	}
	if(errno!=0){
		fprintf(stderr,"Map Region Error: %s\n",strerror(errno));
		remove("./mtext.txt");
		exit(EXIT_FAILURE);
	}
	memcpy(addr,buf,strlen(buf));
	lseek(fd2,offset,SEEK_SET);
	read(fd2,buf,sizeof (char));
	int temp=0;
	if((temp=memcmp(buf,"a",sizeof (char)))==0){
		fprintf(stderr,"Byte Has Changed\n");
		remove("./mtext.txt");
		exit(0);
	}
	else{	
		fprintf(stderr,"Byte Remains the Same\n");
		remove("./mtext.txt");
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
		remove("./mtext.txt");
		exit(EXIT_FAILURE);
	}
	originalSize=(int)testFile.st_size;	
	errno=0;
	addr=mmap(NULL,4100,PROT_READ|PROT_WRITE,MAP_SHARED,fd4,offset);
	if(errno!=0){
		fprintf(stderr,"Map Region Error: %s\n",strerror(errno));
		remove("./mtext.txt");
		exit(EXIT_FAILURE);	
	}
	strcat(buf4,"*");
	memcpy(addr,buf4,strlen(buf4));
	if(b==4){
		if(fstat(fd4,&testFile)<0){
			fprintf(stderr,"Uable to stat mtext.txt, Error: %s\n",strerror(errno));
			remove("./mtext.txt");
			exit(EXIT_FAILURE);
		}
		changedSize=(int)testFile.st_size;
		if(changedSize=originalSize){
			remove("./mtext.txt");
			exit(1);
		}
		else{
			remove("./mtext.txt");
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
			remove("./mtext.txt");
			exit(0);
		}
		else{	
			remove("./mtext.txt");
			exit(1);
		}
	}
}

void mtest6(){
	char buf6[5100];
	char readBuf[2]={'\0','\0'};
	int fd6=0;
	if(!(fd6=open("./mtext.txt",O_RDWR|O_CREAT|O_TRUNC,0666))){
		fprintf(stderr,"Open/Create mtext.txt Error: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	strcat(buf6,"This is a small file.");
	write(fd6,buf6,strlen(buf6));
	errno=0;
	addr=mmap(NULL,8192,PROT_READ,MAP_SHARED,fd6,offset);
	if(errno!=0){
		fprintf(stderr,"Map Region Error: %s\n",strerror(errno));
		remove("./mtext.txt");
		exit(EXIT_FAILURE);
	}
	runHandler();
	memcpy(buf6,addr,64*sizeof(char));
	fprintf(stderr,"A Byte that beyond EOF is %x\n",buf6[50]);
	memcpy(buf6,addr,5000*sizeof(char));
	fprintf(stderr,"A Byte that beyond EOF is %x\n",buf6[4098]);
	remove("./mtext.txt");
	exit(0);
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
		case 4:
			mtest4(4);
			break;
		case 5:
			mtest4(5);
			break;
		case 6:
			mtest6();
			break;
		default:
			exit(1);
	}
}	
	
