#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

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

#define PORT 2000
#define IP "127.0.0.1"
#define FileName "logsocket.txt"


typedef struct{

    char Buffer[30];
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

	SigactionSetup();
	
	struct sockaddr_in addr;
	struct sockaddr_in Server_Address = {0};
    
	int soc_client;
    char *msgptr;
    Message MessageSend;
    char Buffer2[1024] = {0};
    int PID ;
    PID = (int)getpid();
	
	fptr = fopen(FileName, "w");
	if(fptr== NULL)
	{
    	perror("Opening File\n");
	}
    fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr, "\t\t\t\tProcess Info\n");	
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
	
	fprintf(fptr,"Process Name :Client \tProcess PID %d\n",PID);
	fprintf(fptr, "IPC Method : Sockets\n");
	fprintf(fptr, "File Descriptor : %d\n ",fileno(fptr));
    fclose(fptr);
    
    msgptr = (char*)&MessageSend;

    sprintf(MessageSend.Buffer,"From Client PID :%d",PID);
    MessageSend.BufLen = strlen(MessageSend.Buffer);
    MessageSend.USRLED = 1;

    if ((soc_client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("[ERROR] Socket creation\n");
        return -1;
    }
    gettimeofday(&t,NULL);
    fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr, "\t\t\t\tSocket Created\n");	
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	
    fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
    printf("[INFO] Socket Created\n");
  
    Server_Address.sin_family = AF_INET;

    Server_Address.sin_port = htons(PORT);
      

    if(inet_pton(AF_INET, IP, &Server_Address.sin_addr)<=0) 
    {
        printf("Invalid address\n");
        return -1;
    }
  
    if (connect(soc_client, (struct sockaddr *)&Server_Address, sizeof(Server_Address)) < 0)
    {
        printf("Connection Failed \n");
        return -1;
    }

    size_t MessageSize = sizeof(MessageSend);
    while(!killer)
    {
        sleep(1);
        int byte_send = send(soc_client,&MessageSize,sizeof(int), 0);
         gettimeofday(&t,NULL);
        fptr = fopen(FileName, "a");
        fprintf(fptr, "----------------------------------------------------------------------------------------\n");
        fprintf(fptr, "\t\t\t\tCLient Sent Message\n");	
        fprintf(fptr, "----------------------------------------------------------------------------------------\n");
         fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
        printf("Message Sent\n");
        byte_send= send(soc_client , (char*)&MessageSend , sizeof(MessageSend), 0 );
        fclose(fptr);
        if(byte_send < sizeof(MessageSend))
        {
            printf("Overflow\n");
            return 1;
        }
        gettimeofday(&t,NULL);
        fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
        
        gettimeofday(&t,NULL);
        
        int MessageLen;
        
        int Byte_read = read(soc_client, &MessageLen, sizeof(int));

        if(Byte_read == sizeof(int))
        {
           // fprintf(fptr , "Size: %d\n",MessageLen);			
            //printf("[Size: %d\n",MessageLen);
        }	
        else
        {
            printf("[ERROR] Invalid data\n");
            return -1;
        }
        int count = 0;
        while((Byte_read = read(soc_client, Buffer2+count, 1024)) < MessageLen)
        {
                
                count= count + Byte_read;	
        }
        gettimeofday(&t,NULL);
        printf("Message Received\n");
        Message *msgptr= (Message*)Buffer2;
        fptr = fopen(FileName, "a");

        fprintf(fptr, "----------------------------------------------------------------------------------------\n");
        fprintf(fptr, "\t\t\t\tCLient Received Message\n");	
        fprintf(fptr, "----------------------------------------------------------------------------------------\n");
        fprintf(fptr,"Message Received  from Client :\nMessage: %s\nLength: %d\nUSRLED: %d\n",msgptr->Buffer,msgptr->BufLen,msgptr->USRLED);
        //printf("[INFO] Message Recvd\nMessage: %s\nMessageLen: %d\nUSRLED: %d\n",msgptr->Buffer,msgptr->BufLen,msgptr->USRLED);
        fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
        fclose(fptr);
    }
    fptr = fopen(FileName, "a");
    fprintf(fptr,"SIGINT RECEIVED\n");
    fclose(fptr);
    close(soc_client);
    return 0;
}