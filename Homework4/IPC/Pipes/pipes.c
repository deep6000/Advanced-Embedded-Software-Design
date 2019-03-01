\/*
*File Name: pthreads.c
*Description: InterProcess Communication between 2 process using pipes  
*Author: Deepesh Sonigra
*Date: 02/26/2019
These sites were reffered to understand the concepts
https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_pipes.htm
https://www.softprayog.in/programming/interprocess-communication-using-pipes-in-linux
*/


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


#define LOGFILE "logfile.txt"

typedef struct {
	char Buffer[20];
	int BufLen;
	bool USRLED;
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
	SigactionSetup();
	int PipeP2C[2];
	int PipeC2P[2];
	struct timeval t;
	pid_t process;
	char* message_PID= "From PID";
	Message ChildMessage = {0}, ParentMessage= {0};
	Message *Childptr, *Parentptr; 
	char ChildReadBuf[sizeof(Message)] = {0};
	char ParentReadBuf[sizeof(Message)] = {0};
	FILE *fptr;


	fptr= fopen(LOGFILE,"w");
	if(fptr == NULL)
	{
	perror("Error:Opeining Log File");
	}

	if (pipe(PipeP2C)==-1)
    	{
    	perror("Pipe Parent to Child Failed");
    	return 1;
    	}
    	if (pipe(PipeC2P)==-1)
   	{
    	perror("Pipe Child to Parent Failed\n");
    	return 1;
    	}
	while(1)
	{
	if(killer)
	break;
		sleep(1);
		process = fork();
		if(process == -1)
		perror("FORK ERROR");
		

		if(process == 0) //Child Process
		{		
		close(PipeP2C[1]);
		close(PipeC2P[0]);
		gettimeofday(&t,NULL);
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		fprintf(fptr, "\t\t\t\tProcess Info\n");	
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
		//printf("Process PID %d\n", getpid());
		fprintf(fptr,"Process Name : Child\tProcess PID %d\n", getpid());
		fprintf(fptr, "IPC Method : Pipes\n");
		fprintf(fptr, "File Descriptor : %d\n ",fileno(fptr));
		
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");	
		fprintf(fptr, "\t\t\tChild : Reading from the Parent process\n" );
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		gettimeofday(&t,NULL);
		read(PipeP2C[0], ChildReadBuf, sizeof(Message));
		Childptr = (Message*)ChildReadBuf;
		fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
		fprintf(fptr, "Message Received - %s \n Message Length - %d \n USRLED - %d\n", Childptr->Buffer,Childptr->BufLen,Childptr->USRLED);
		
		sprintf(ChildMessage.Buffer,"From PID %d", getpid());
		ChildMessage.BufLen= strlen(ChildMessage.Buffer);
		ChildMessage.USRLED = 1;
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		fprintf(fptr, "\t\t\tChild : Sending Message to the Parent process\n\n" );
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		
		gettimeofday(&t,NULL);
		write(PipeC2P[1],(char*)&ChildMessage,sizeof(ChildMessage));
		fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
		fprintf(fptr, "Message sent to Parent\n");
		close(PipeC2P[1]);
		//exit(0);
		}

		else if(process > 0)
		{
		close(PipeP2C[0]);
		sprintf(ParentMessage.Buffer,"From PID %d", getpid());
		ParentMessage.BufLen= strlen(ParentMessage.Buffer);
		ParentMessage.USRLED = 0;
		
		gettimeofday(&t,NULL);
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		fprintf(fptr, "\t\t\t\tProcess Info\n");	
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
		//printf("Process Name : Parent\tProcess PID %d\n", getpid());
		fprintf(fptr,"Process Name : Parent\tProcess PID %d\n", getpid());
		fprintf(fptr, "IPC Method : Pipes\n");
		fprintf(fptr, "File Descriptor : %d\n ",fileno(fptr));
		
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		fprintf(fptr, "\t\t\tParent : Sending Message to the Child process\n\n" );	
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		gettimeofday(&t,NULL);
		write(PipeP2C[1],(char*)&ParentMessage,sizeof(ParentMessage));
		fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
		fprintf(fptr, "Message Sent to Child Process\n");
		close(PipeP2C[1]);

		// waiting to child to send a message
		wait(NULL);
		// once message received close child2parent write pipe
		close(PipeC2P[1]);

		// Start Reading messages from child

		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		fprintf(fptr, "\t\t\tParent : Receiving Message from the Child process\n\n" );	
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		gettimeofday(&t,NULL);
		read(PipeC2P[0], ParentReadBuf, sizeof(Message));
		Parentptr = (Message*)ParentReadBuf;
		fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
		fprintf(fptr, "Message Received - %s \n Message Length - %d \n USRLED - %d\n", Parentptr->Buffer,Parentptr->BufLen,Parentptr->USRLED);
		close(PipeC2P[0]);	
		//exit(0);
			
		}

		
   }
	
		
		fprintf(fptr, "Exiting Pipes\n");
		fprintf(fptr, "Received SIGINT\n");
		fclose(fptr);
		exit(0);		

}	

	