#include <sys/mman.h>
#include <sys/stat.h>        		   
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

#define FileName "logshared.txt"

#define SHARED_MEM_NAME	"/Shared_mem"
#define SHARED_MEM_SIZE sizeof(Message)

#define SEMAPHORE "/sharedMemSemaphore"



typedef struct{
    char Buffer[256];
    int BufLen;
    bool USRLED;
}Message;


int main()
{

	struct timeval t;
	FILE *fptr;
	fptr = fopen(FileName, "a");
	Message MessageReceive = {0};
	Message MessageSend = {0};
    
	if(fptr== NULL)
	{
	perror("Opening File\n");
	}
	printf("[INFO] Starting the process 2\n");	

	gettimeofday(&t,NULL);
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr, "\t\t\t\tProcess Info\n");	
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
		//printf("Process PID %d\n", getpid());
	fprintf(fptr,"Process Name : Process 2 \tProcess PID %d\n", getpid());
	fprintf(fptr, "IPC Method : Shared memory\n");
	fprintf(fptr, "File Descriptor : %d\n ",fileno(fptr));

	int Shared_mem = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
	if(Shared_mem < 0 )
	{
		printf("[ERROR] Shared Mem);
		return -1;
	}

	void *shared_mem = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED , Shared_mem, 0);
	if(shared_mem == (void*)-1)
	{
		printf("[ERROR] mmap error");
		return -1;
	}

	
	sem_t *sem = sem_open(SEMAPHORE, O_CREAT, 0666, 0);
	if(SEM_FAILED == sem)
	{
		printf("[ERROR] Sem open Failed:%s\n",strerror(errno));
		return -1;	
	}
	sem_wait(sem);

char *msgptr = (char*)&MessageReceive;

	gettimeofday(&t,NULL);
    memcpy(msgptr,(char*)shared_mem,SHARED_MEM_SIZE);

    fprintf(fptr, "----------------------------------------------------------------------------------------\n");	
	fprintf(fptr, "\t\t\t Process 2: Reading from Shared memory\n" );
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");

	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
    fprintf(fptr," Message From Process 1 Shared Memory \n{Message: %s\nMessageLen: %d\nUSRLED: %d}\n",MessageReceive.Buffer,MessageReceive.BufLen,MessageReceive.USRLED);

 
    
    msgptr = (char*)&MessageSend;

    sprintf(MessageSend.Buffer,"Hello this is Process 2  PID %d",getpid());
    MessageSend.BufLen = strlen(MessageSend.Buffer);
    MessageSend.USRLED = 1;
	gettimeofday(&t,NULL);
  fprintf(fptr, "----------------------------------------------------------------------------------------\n");	
	fprintf(fptr, "\t\t\t Process 2 : Copying to Shared memory\n" );
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");

	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
   
    memcpy((char*)shared_mem, msgptr, SHARED_MEM_SIZE);

  
    sem_post(sem);

	
	close(Shared_mem);
	
	
	return 0;
}
