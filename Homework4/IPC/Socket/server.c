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

#define PORT 	2000
#define IP		
#define FileName "logsocket.txt"

typedef struct{
    char Buffer[20];
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
	SigactionSetup();
	struct timeval t;
	FILE *fptr;
	
	
	Message MessageReceive = {0};
	Message MessageSent = {0};
	int soc_server;
	int accepted_socket, Byte_read;
	char Buffer2[1024] = {0};
	char PeerIP_Addr[30] = {0};
	int MessageLen = 0;
	int count=0;
	int PID ;
	PID=  (int)getpid();



	if(fptr== NULL)
	{
	perror("Opening File\n");
	}
	gettimeofday(&t,NULL);
	fptr = fopen(FileName, "a");
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr, "\t\t\t\tProcess Info\n");	
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
	fprintf(fptr,"Process Name : Server \tProcess PID %d\n", PID);
	fprintf(fptr, "IPC Method : Socket\n");
	fprintf(fptr, "File Descriptor : %d\n ",fileno(fptr));
	fclose(fptr);
	
	

	
	struct sockaddr_in addr, Peer_address;
	
	
	if((soc_server = socket(AF_INET,SOCK_STREAM,0)) == 0)
	{
	gettimeofday(&t,NULL);
	fptr = fopen(FileName, "a");
	
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");	
	fprintf(fptr, "\t\t\t Socket Created\n" );
	fprintf(fptr, "----------------------------------------------------------------------------------------\n");
	fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
	fclose(fptr);
	
		printf("[ERROR] Socket Creation\n");
		return 1;
	}
	int option = 1;
	printf("Socket Created\n");

	if (setsockopt(soc_server, SOL_SOCKET, SO_REUSEADDR, &(option), sizeof(option)))
    {
        printf("[ERROR] Cannot Set socket options\n");
        return 1;
    }
	/*Setting up the sockaddr_in structure */
	addr.sin_family = AF_INET;
	/* Change the below address to our IP addr */
	//addr.sin_addr.s_addr = inet_addr("192.168.1.238");//INADDR_ANY;	
	addr.sin_addr.s_addr = INADDR_ANY;	//Using local loopback	
	addr.sin_port = htons(PORT);

	
	if((bind(soc_server,(struct sockaddr*)&addr, sizeof(addr))) < 0)
	{
	gettimeofday(&t,NULL);
		printf("[ERROR] Cannot bind the socket\n");
		return 1;
	}

	
	
	printf("[INFO] Socket binded\n");


	if(listen(soc_server,5) < 0)
	{
		printf("[ERROR] Cannot listen\n");
		return 1;
	}
	printf("Socket Listening\n");
	int Addr_length = sizeof(Peer_address);
	
	accepted_socket = accept(soc_server, (struct sockaddr*)&Peer_address,(socklen_t*)&Addr_length);
	gettimeofday(&t,NULL);
		
		
	
	while (!killer)
	{
		sleep(1);
		Byte_read = read(accepted_socket, &MessageLen, sizeof(int));

		if(Byte_read == sizeof(int))
		{
					
		}	
		else
		{
			printf("[ERROR] Invalid data\n");
			return -1;
		}
		
		while((Byte_read = read(accepted_socket, Buffer2+count, 1024)) < MessageLen)
		{
			
			count= count + Byte_read;	
		}
		gettimeofday(&t,NULL);
		Message *msgptr= (Message*)Buffer2;
		fptr = fopen(FileName, "a");
		printf("Message Received\n");
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		fprintf(fptr, "\t\t\t\tServer Received Message\n");	
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		fprintf(fptr,"Message: %s\nLength: %d\nUSRLED: %d\n",msgptr->Buffer,msgptr->BufLen,msgptr->USRLED);
		fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
		fclose(fptr);
	
   		sprintf(MessageSent.Buffer,"From Server PID :%d", PID);
   		MessageSent.BufLen = strlen(MessageSent.Buffer);
    	MessageSent.USRLED = 1;

		size_t MessageSize = sizeof(MessageSent);

 		 int byte_send = send(accepted_socket,&MessageSize,sizeof(int), 0);
   

    	byte_send= send(accepted_socket , (char*)&MessageSent , sizeof(MessageSent), 0 );

   		 if(byte_send < sizeof(MessageSent))
    	{
        printf("Overflow\n");
        return 1;
    	}
    	gettimeofday(&t,NULL);
		fptr = fopen(FileName, "a");
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		fprintf(fptr, "\t\t\t\tServer Sent Message\n");	
		fprintf(fptr, "----------------------------------------------------------------------------------------\n");
		fprintf(fptr,"Time: %lu Seconds %lu Microseconds\n",t.tv_sec,t.tv_usec );
		fclose(fptr);
		printf("Message Sent\n");
   }
	fptr = fopen(FileName, "a");
	fprintf(fptr,"SIGINT RECEIVED");
	fclose(fptr);
	close(accepted_socket);

	return 0;
}