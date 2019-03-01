
/*https://stackoverflow.com/questions/3056307/how-do-i-use-mqueue-in-a-c-program-on-a-linux-based-system*/

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


#define MAX_MESSAGES (10)
#define QUEUENAME "/my_queue"
#define FileName "Logqueue.txt"

typedef struct 
{
	char Buffer[256];
	uint8_t BufLen;
	bool USRLED;
}Message;


int main()
{
	struct timeval t;
	FILE *fptr;
	fptr = fopen(FileName, "a");
	if(fptr== NULL)
	{
	perror("Opening File\n");
	}

	char *msgsendptr, *msgrecptr;
	Message MessageSend= {0};
	Message MessageReceive= {0};
	struct mq_attr QueueAttr;
	QueueAttr.mq_flags = 0;
	QueueAttr.mq_maxmsg = MAX_MESSAGES;
	QueueAttr.mq_msgsize = sizeof(Message);

	mqd_t mq = mq_open(QUEUENAME, O_CREAT |  O_RDWR, 0666, &QueueAttr);


	if( mq== (mqd_t)-1)
	{	
		printf("ERRO QUEUE OPEN\n");
		return -1;
	}
 	gettimeofday(&t,NULL);
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr, "\t\t\t\tProcess Info\n");	
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
		//printf("Process PID %d\n", getpid());
	fprintf(fptr,"Process Name : Process 1 \tProcess PID %d\n", getpid());
	fprintf(fptr, "IPC Method : Queues\n");
	fprintf(fptr, "File Descriptor : %d\n ",fileno(fptr));
	
	msgsendptr = (char*)&MessageSend;
	sprintf(MessageSend.Buffer,"From Process 1 PID %d Hello This is Process 1 ", getpid());
	MessageSend.BufLen= strlen(MessageSend.Buffer);
	MessageSend.USRLED = true;

	gettimeofday(&t,NULL);
	int status = mq_send(mq ,msgsendptr, sizeof(MessageSend),0);
	if(status == -1)
	{
		perror("Error Send Message\n");
		return -1;
	}

	fprintf(fptr, "----------------------------------------------------------------------------------------\n");	
		fprintf(fptr, "\t\t\t Sending to Process 2\n" );
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");

	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );	
	//printf("Message Sending to Process 2 \n Message = %s\n  Length = %d\n USRLED = %d\n", MessageSend.Buffer,MessageSend.BufLen, MessageSend.USRLED);
	//fprintf(fptr,"Message Sending to Process 2 \n Message = %s\n Length = %d\n USRLED = %d\n", MessageSend.Buffer,MessageSend.BufLen, MessageSend.USRLED);	

	
	fprintf(fptr,"\n\nWaiting for process 2\n");

	msgrecptr= (char*)&MessageReceive;

	
	status = mq_receive(mq, msgrecptr, sizeof(MessageReceive),0);
	if(status == -1)
	{
		printf("[ERROR] Q Send error");
		return -1;
	}
	gettimeofday(&t,NULL);

	fprintf(fptr, "----------------------------------------------------------------------------------------\n");	
		fprintf(fptr, "\t\t\t Receiving From Process 2\n" );
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");

	printf("Message received from Process 2 \n Message = %s\n Length = %d\n USRLED = %d\n", MessageReceive.Buffer,MessageReceive.BufLen, MessageReceive.USRLED);

	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
	fprintf(fptr,"Message received from Process 2 \n Message = %s\n , Length = %d\n USRLED = %d\n", MessageReceive.Buffer,MessageReceive.BufLen, MessageReceive.USRLED);

	mq_close(mq);
	mq_unlink(QUEUENAME);
	gettimeofday(&t,NULL);

	fprintf(fptr, "----------------------------------------------------------------------------------------\n");	
		fprintf(fptr, "\t\t\t queue Destroyed\n" );
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
	printf("QUEUE destroyed\n");
	return 0;


}
