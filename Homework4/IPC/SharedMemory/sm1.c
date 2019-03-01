#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
		        /* For O_* constants */

#include <errno.h>

#include <pthread.h>


#include <sys/stat.h>        
#include <mqueue.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include<sched.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include<fcntl.h>
 #include <sys/time.h>

#define SHARED_MEMORY_NAME	"/MY_SH_MEM"
#define SHARED_MEMORY_SIZE sizeof(Message)

#define SEMAPHORE_NAME "/sem_shared_mem"
#define FileName "logshared.txt"



typedef struct{
    char Buffer[256];
    int BufLen;
    uint8_t USRLED:1;
}Message;
int killer = 0;

void KILLFUNC(int signum)
{
killer =1 ;
}

int SigactionSetup()
{

 	
	struct sigaction SigAct;

	memset(&SigAct, 0, sizeof(SigAct));
	SigAct.sa_handler= KILLFUNC;


	if(sigaction(SIGINT,&SigAct,NULL)== -1)
	perror("Sigaction Failed");
	
}
int main()
{
	struct timeval t;
	FILE *fptr;
	fptr= fopen(FileName,"a");
	SigactionSetup();
	if(fptr == NULL)
	{
	perror("Error:Opening printf File");
	}
	fprintf(fptr, "Starting Process 1\n");
	printf("[INFO] Starting the process 1\n");	
	int SharedMem = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
	if(SharedMem < 0)
	{
		printf("[ERROR] Cannot open Shared Memory");
		return -1;
	}
	gettimeofday(&t,NULL);
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr, "\t\t\t\tProcess Info\n");	
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
	fprintf(fptr,"Process Name : Process 1 \tProcess PID %d\n", getpid());
	fprintf(fptr, "IPC Method : Queues\n");
	fprintf(fptr, "File Descriptor : %d\n ",fileno(fptr));
	int status = ftruncate(SharedMem, SHARED_MEMORY_SIZE);
	if(status < 0)
	{
		perror(" Ftruncate\n");
		return -1;
	}

	void *shared_mem = mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED , SharedMem, 0);
	if(shared_mem == (void*)-1)
	{
		perror("mmap error\n");
		return -1;
	}

	
	sem_t *sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 0);
	if(SEM_FAILED == sem)
	{
		printf("[ERROR] Sem open Failed:%s\n",strerror(errno));
		return -1;	
	}

    char *msgptr;
    Message MessageSend = {0};
    msgptr = (char*)&MessageSend;

    sprintf(MessageSend.Buffer,"Hello from Process1 from PID %d",getpid());
    MessageSend.BufLen = strlen(MessageSend.Buffer);
    MessageSend.USRLED = 1;

		while(!killer)
	{	
	sleep(1);
gettimeofday(&t,NULL);
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");	
		fprintf(fptr, "\t\t\t Process 1 :  Copying  to Shared memory\n" );
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");

	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );	
	
    //Copy to Shared Memory
    memcpy((char*)shared_mem, msgptr, SHARED_MEMORY_SIZE);


    sem_post(sem);
    sem_wait(sem);

    Message MessageReceive = {0};
    msgptr = (char*)&MessageReceive;

    memcpy(msgptr,(char*)shared_mem,SHARED_MEMORY_SIZE);

    fprintf(fptr, "----------------------------------------------------------------------------------------\n");	
		fprintf(fptr, "\t\t\t Process 1 : Reading from Shared memory\n" );
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");

	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
    fprintf(fptr,"Message From Process 2 through Shared Mem\n{Message: %s\nMessageLen: %d\nUSRLED: %d}\n",MessageReceive.Buffer,MessageReceive.BufLen,MessageReceive.USRLED);


	fprintf(fptr,"SIGINT RECEIVED\n");
	
	 shm_unlink(SHARED_MEMORY_NAME);
	

	return 0;
}