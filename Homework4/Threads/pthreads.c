/*
*File Name: pthreads.c
*Description: Creating Multiple threads
			  Thread1: to open a file, count characters in the file log it to a file if count greater than 100
			  Thread2: Report CPU Utilization every 100ms and log the same file  
*Author: Deepesh Sonigra
*Date: 02/22/2019
****This site was reffered to understand the concepts of creating and managing mulitple threads****
*Link: https://www.thegeekstuff.com/2012/04/create-threads-in-linux/

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include<sched.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <semaphore.h>
#include <sys/time.h>
#include <signal.h>


/*Define Macros*/
#define ThreadsCount 		(2)
#define MAXCHARACTERS       	(26)
#define TIMEINTERVAL_MS		(100) 
#define NO_OF_LINES   		(5)

/* Functions */
int LowerCase(char ch);
void CharacterCount(FILE *Fptr, int arr[MAXCHARACTERS]);

/* Global Structures*/
typedef struct
{
	char *FileName;
}StructThread;

StructThread FileLog ;
/*Global variables*/
pthread_mutex_t mutx;
pthread_t threads[ThreadsCount];
sem_t TimerExpire;
int count=0;
int flag;
timer_t TimerID;
int usr1 , usr2;



/******************************************************************************************************************************************/
int LowerCase(char ch)
 {
    if ((ch >= 'A') &&  (ch <= 'Z'))
         ch = ch + 32;
    return ch;
 }
/******************************************************************************************************************************************/
void CharacterCount(FILE *Fptr, int Array[MAXCHARACTERS])
{
	char ch;
  	ch = fgetc(Fptr);
	while (!feof(Fptr))	
	{
		  
		ch = LowerCase(ch);
		if( ch >= 'a' && ch <= 'z')
		{
			Array[ch-'a'] += 1;
		} 
	ch = fgetc(Fptr);	
	}

}


/******************************************************************************************************************************************/
void TimerISR(union sigval sv)
{
	count++;	
	flag =1;
	}

/******************************************************************************************************************************************/
int Mask(){
/*http://man7.org/linux/man-pages/man3/pthread_sigmask.3.html*/

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	sigaddset(&set,SIGINT);

	pthread_sigmask(SIG_BLOCK, &set, NULL);

	return 0;

}


void SignalHandler(int signal, siginfo_t *siginfo, void *ucontext)
{
	//time_t t = time(NULL);
	//struct timeval ts;
	FILE *Filesig = fopen(FileLog.FileName,"a");
	int status;	

	if(signal == SIGUSR1)
	{
	usr1= 1;
	/*
	gettimeofday(&ts,NULL);
	struct tm endtm = *localtime(&t);
        printf("USR1 signal received\n");
        printf("Leaving child threads\n");

	fprintf(Filesig,"\n\nChild Thread [1]: Terminated  USR1\t");
	fprintf(Filesig,"Time: %lu Seconds %lu Microseconds\n",ts.tv_sec,ts.tv_usec );
	fprintf(Filesig,"\n\nChild Thread [2]: Terminated  USR1\t");
	fprintf(Filesig,"Time: %lu Seconds %lu Microseconds\n",ts.tv_sec,ts.tv_usec );
	fprintf(Filesig, " Date\t\t Time\t\tThread Name\t\tPOSIXID \t\tTID\n");
	fprintf(Filesig, "%d-%d-%d\t%d:%d:%d\t\tChild 1\t\t%lu\t\t%d\n",endtm.tm_year + 1900, endtm.tm_mon + 1, endtm.tm_mday, endtm.tm_hour, endtm.tm_min, endtm.tm_sec,(pthread_t)pthread_self(), (pid_t)syscall(SYS_gettid));
	fclose(Filesig);
	status= timer_delete(TimerID);
	if(status == -1)
	perror("timer_delete: ");
        pthread_cancel(threads[0]);
        pthread_cancel(threads[1]); */
}

	else if(signal == SIGUSR2)
	{
	usr2 =1;
	/*	
	gettimeofday(&ts,NULL);
	struct tm endtm = *localtime(&t);
	fprintf(Filesig, "USR2 signal received\n");
        printf("SIG_HL:\t\tUSR2 signal received\n");
        fprintf(Filesig, "SIG_HL:\t\tLeaving child thread\n");
        printf("SIG_HL:\t\tLeaving child threads\n");
	fprintf(Filesig,"\n\nChild Thread [1]: Terminated  USR2\t");
	fprintf(Filesig,"Time: %lu Seconds %lu Microseconds\n",ts.tv_sec,ts.tv_usec );
	fprintf(Filesig,"\n\nChild Thread [2]: Terminated  USR2\t");
	fprintf(Filesig,"Time: %lu Seconds %lu Microseconds\n",ts.tv_sec,ts.tv_usec );
	fprintf(Filesig, " Date\t\t Time\t\tThread Name\t\tPOSIXID \t\tTID\n");
	fprintf(Filesig, "%d-%d-%d\t%d:%d:%d\t\tChild 1\t\t%lu\t\t%d\n",endtm.tm_year + 1900, endtm.tm_mon + 1, endtm.tm_mday, endtm.tm_hour, endtm.tm_min, endtm.tm_sec,(pthread_t)pthread_self(), (pid_t)syscall(SYS_gettid));
        fclose(Filesig);
	status= timer_delete(TimerID);
	if(status == -1)
	perror("timer_delete: ");
        pthread_cancel(threads[0]);
        pthread_cancel(threads[1]);
*/
	}


}


int SigactionSetup()
{

 	
	struct sigaction SigAct;

	memset(&SigAct, 0, sizeof(SigAct));
	SigAct.sa_sigaction = SignalHandler;
	SigAct.sa_flags = SA_SIGINFO;

	if(sigaction(SIGUSR1,&SigAct,NULL)== -1)
	perror("Sigaction Failed: USR1");
	
	if(sigaction(SIGUSR2,&SigAct,NULL)== -1)
	perror("Sigaction Failed:USR2");

	/*if(sigaction(SIGINT,&SigAct,NULL)== -1)
	perror("Sigaction Failed");
	*/
}

/******************************************************************************************************************************************/
void* Child1Thread(void *arg)
{
	int CharacterArray[MAXCHARACTERS]= {0};
	time_t t = time(NULL);
	struct timeval ts;
 	
	Mask();

	//Declare a pointer to the struct Thread
	StructThread *Thread1ptr = (StructThread*)(arg);
	FILE *LogFileptr, *IpFileptr;

	struct tm starttm= *localtime(&t);

	
	gettimeofday(&ts,NULL);
	

	printf("Child1thread started, POSIX %lu TID %d\n:", (pthread_t)pthread_self(), (pid_t)syscall(SYS_gettid));
//	sleep(2); // Keep it alive so we're sure the second thread gets a unique ID
	//pthread_mutex_lock(&mutx);
	LogFileptr = fopen(Thread1ptr->FileName, "a");
	if(LogFileptr == NULL)		
	{
		perror("Error: Cannot open file\n");
		printf("Error\n");
		exit(1);
	} 
	printf( "Child Thread [1]: Started\n ");
	fprintf(LogFileptr,"\n\nChild Thread [1]: Started\t ");
	fprintf(LogFileptr,"Time: %lu Seconds %lu Microseconds\n",ts.tv_sec,ts.tv_usec );
	fprintf(LogFileptr, "  Date\t\t Time\t\tThread Name\t\tPOSIXID \t\tTID\n");
	fprintf(LogFileptr, "%d-%d-%d\t%d:%d:%d\t\tChild 1\t\t%lu\t\t%d\n",starttm.tm_year + 1900, starttm.tm_mon + 1, starttm.tm_mday, starttm.tm_hour, starttm.tm_min, starttm.tm_sec,(pthread_t)pthread_self(), (pid_t)syscall(SYS_gettid));

	IpFileptr = fopen("gdb.txt","r");
	if(IpFileptr == NULL)	
	{
		perror("Error: Cannot open file\n");
		printf("Error\n");		
		exit(1);
	} 
	CharacterCount(IpFileptr,CharacterArray);
	
	fclose(IpFileptr);
	fprintf(LogFileptr,"\n--------------------Printing Character Frequencies(<100) [a-z]--------------------\n ");
	for(int letter=0;letter<MAXCHARACTERS;letter++)
	{
		if(usr1 | usr2)
		break;
		if(CharacterArray[letter] >= 1 && CharacterArray[letter] < 100)
			fprintf(LogFileptr,"\t\t\tCharacter [%c]\t\tCount: [%d]\n",(letter+'a'), CharacterArray[letter]);
	}
	
	struct tm endtm = *localtime(&t);
	gettimeofday(&ts,NULL);
	if(usr1)
	fprintf(LogFileptr,"\n\nChild Thread [1]: Terminated USR1\t");
	else if(usr2)
	fprintf(LogFileptr,"\n\nChild Thread 1]: Terminated USR2\t");
	else{
	printf( "Child Thread [1]: Task Completed\n ");
	fprintf(LogFileptr,"\n\nChild Thread [1]: Terminated\t");}
	fprintf(LogFileptr,"Time: %lu Seconds %lu Microseconds\n",ts.tv_sec,ts.tv_usec );
	fprintf(LogFileptr, " Date\t\t Time\t\tThread Name\t\tPOSIXID \t\tTID\n");
	fprintf(LogFileptr, "%d-%d-%d\t%d:%d:%d\t\tChild 1\t\t%lu\t\t%d\n",endtm.tm_year + 1900, endtm.tm_mon + 1, endtm.tm_mday, endtm.tm_hour, endtm.tm_min, endtm.tm_sec,(pthread_t)pthread_self(), (pid_t)syscall(SYS_gettid));
	
	fflush(LogFileptr);
	fclose(LogFileptr);
	
	//pthread_mutex_unlock(&mutx);
	pthread_exit(NULL);
}
/******************************************************************************************************************************************/
void* Child2Thread(void *arg)
{	
	
	time_t t = time(NULL);
	struct timeval ts;
	char Line2Read[50];
	
	unsigned timeout = TIMEINTERVAL_MS;
	int status;
	struct sigevent TimerSigEvt;
	struct itimerspec TimerPeriod; 	
	struct sigaction signal2;

	//Declare a pointer to the struct Thread
	StructThread *Thread2ptr = (StructThread*)(arg);
	FILE *LogFile2ptr, *IpFile2ptr;

	gettimeofday(&ts,NULL);
	struct tm starttm = *localtime(&t);



	printf("Child2thread started, POSIX %lu TID %d\n", (pthread_t)pthread_self(), (pid_t)syscall(SYS_gettid));
	


	LogFile2ptr = fopen(Thread2ptr->FileName, "a");
	if(LogFile2ptr == NULL)
	{
		printf("Error\n");
		perror("Error: Cannot open file\n");
		exit(1);
	} 
	
	printf("Child Thread [2]: Started\n");
	fprintf(LogFile2ptr,"\n\nChild Thread [2]: Started\t");
	fprintf(LogFile2ptr,"Time: %lu Seconds %lu Microseconds\n",ts.tv_sec,ts.tv_usec );

	fprintf(LogFile2ptr, "  Date\t\t Time\t\tThread Name\t\tPOSIXID \t\tTID\n");
	fprintf(LogFile2ptr, "%d-%d-%d\t%d:%d:%d\t\tChild 2\t\t%lu\t\t%d\n",starttm.tm_year + 1900, starttm.tm_mon + 1, starttm.tm_mday, starttm.tm_hour, starttm.tm_min, starttm.tm_sec,(pthread_t)pthread_self(), (pid_t)syscall(SYS_gettid));
	
	
	/* Set all `SigEvt` and `trigger` memory to 0 */
        memset(&TimerSigEvt, 0, sizeof(struct sigevent));
        memset(&TimerPeriod, 0, sizeof(struct itimerspec));

        TimerSigEvt.sigev_notify = SIGEV_THREAD;
        TimerSigEvt.sigev_notify_function = TimerISR;
      //  TimerSigEvt.sigev_value.sival_int = count;
	TimerSigEvt.sigev_notify_attributes = NULL;
	
	TimerPeriod.it_value.tv_sec  = timeout / 1000;
	TimerPeriod.it_value.tv_nsec = ((timeout %1000)*1000000);
	TimerPeriod.it_interval.tv_sec  = timeout/1000;
	TimerPeriod.it_interval.tv_nsec = ((timeout%1000)*1000000);

	 status= timer_create(CLOCK_MONOTONIC, &TimerSigEvt,&TimerID);
	if (status == -1 )
		printf("ERROR - creating a new timer failed\n");	
	
	status = timer_settime(TimerID, 0, &TimerPeriod, 0);
	if((status) == -1)
	printf("ERROR - setting up a period of the timer\n");
		

	while(count < 150)
	{
		if(usr1 | usr2)
			break;
		if(flag)
		{
	
			IpFile2ptr = fopen("/proc/stat","r");
		if(IpFile2ptr ==  NULL)
		{
			printf("Error\n");
			perror("Error: Cannot open file\n");
			exit(1);
		} 
			fprintf(LogFile2ptr,"\n------------------------------CPU UTILIZATION------------------------------\n");	
			fprintf(LogFile2ptr,"%d] ",count+1);
			for(int i=0;i<1;i++)
			{
				fgets(Line2Read, sizeof (Line2Read),IpFile2ptr);
				fprintf(LogFile2ptr,"%s",Line2Read);
			}
		flag= 0;		
		fclose(IpFile2ptr);
		}
	}
	count = 0;
	struct tm endtm = *localtime(&t);
	gettimeofday(&ts,NULL);
	
	if(usr1)
	{
	printf("Child Thread 2 Terminated USR1\n");
	fprintf(LogFile2ptr,"\n\nChild Thread [2]: Terminated USR1\t");
	}	
	else if(usr2)
	{
	printf("Child Thread 2 Terminated USR2\n");
	fprintf(LogFile2ptr,"\n\nChild Thread [2]: Terminated USR2\t");
	}
	else
	{
	printf( "Child Thread [2]: Task Completed");	
	fprintf(LogFile2ptr,"\n\nChild Thread [2]: Terminated\t");
	}	
	status = timer_delete(TimerID);
	if(status == -1)
	perror("timer_delete: ");



	fprintf(LogFile2ptr,"Time: %lu Seconds %lu Microseconds\n",ts.tv_sec,ts.tv_usec );
	fprintf(LogFile2ptr, "  Date\t\t Time\t\tThread Name\t\tPOSIXID \t\tTID\n");
	fprintf(LogFile2ptr, "%d-%d-%d\t%d:%d:%d\t\tChild 2\t\t%lu\t\t%d\n",endtm.tm_year + 1900, endtm.tm_mon + 1, endtm.tm_mday, endtm.tm_hour, endtm.tm_min, endtm.tm_sec,(pthread_t)pthread_self(), (pid_t)syscall(SYS_gettid));
	
	//pthread_mutex_unlock(&mutx);
fflush(LogFile2ptr);
fclose(LogFile2ptr);	
	pthread_exit(NULL);
}

/******************************************************************************************************************************************/
int main(int argc, char **argv)
{

	
	int err;
	time_t t = time(NULL);
	struct timeval ts;
	

	StructThread MyThreads[ThreadsCount];
	FILE *LogMasterptr;	
	//pthread_mutex_init(&mutx,NULL);

	if(argc < 2 )
		printf("Enter the Filename for logging data \n");
	//pthread_mutex_lock(&mutx);
	
	struct tm starttm = *localtime(&t);
	gettimeofday(&ts,NULL);		
	LogMasterptr = fopen(argv[1], "w+");
	FileLog.FileName = argv[1];

	if(LogMasterptr == NULL)		
	{
		printf("Error\n");
		perror("Error: Cannot open file\n");
		exit(1);
	} 
	SigactionSetup();
	
	printf("Master Started\t");
	
	printf("PID : %d/n",getpid());
	fprintf(LogMasterptr,"\n\nMAster Started \n");
	fprintf(LogMasterptr,"Time: %lu Seconds %lu Microseconds\n",ts.tv_sec,ts.tv_usec );
	fprintf(LogMasterptr, "  Date\t\t Time\t\tThread Name\t\tPOSIXID\t\tTID\n");
	fprintf(LogMasterptr, "%d-%d-%d\t%d:%d:%d\t\tMaster\t\t%lu\t\t%d\n",starttm.tm_year + 1900, starttm.tm_mon + 1, starttm.tm_mday, starttm.tm_hour, starttm.tm_min, starttm.tm_sec,(pthread_t)pthread_self(), (pid_t)syscall(SYS_gettid));

	sem_init(&TimerExpire,0,0);

	for(int i =0; i<ThreadsCount;i++)
		MyThreads[i].FileName = argv[1];

	
 	err= pthread_create(&threads[0],NULL, Child1Thread, (void *)&(MyThreads[0]));// parameters to pass in
	if(err != 0)	
	{
		perror("\nError! Could not create thread:1\n ");
		//pthread_mutex_unlock(&mutx);
		exit(1);

	}

	err = pthread_create(&threads[1], NULL, Child2Thread, (void *)&(MyThreads[1]));// parameters to pass in
	if(err != 0)	
	{
		perror("\nError! Could not create thread: 2\n ");
		//pthread_mutex_unlock(&mutx);
		exit(1);
	}

/*Main threads waits for two child threads to complete the execution*/
	for(int index = 0; index < ThreadsCount; index++)
	{

		if(pthread_join(threads[index], NULL))
		{
			printf("Error in pthread join\n");
		}
	}


	

	struct tm endtm = *localtime(&t);
	gettimeofday(&ts,NULL);
	LogMasterptr = fopen(argv[1], "a");
	printf("\nMasterTHread Exited\t:w");
fprintf(LogMasterptr,"Time: %lu Seconds %lu Microseconds\n",ts.tv_sec,ts.tv_usec );
	fprintf(LogMasterptr,"\n\nMaster Thread Terminated \n");
	fprintf(LogMasterptr, "  Date\t\t Time\t\tThread Name\t\tPOSIXID \t\tTID\n");
	fprintf(LogMasterptr, "%d-%d-%d\t%d:%d:%d\t\tMaster\t\t%lu\t\t%d\n",endtm.tm_year + 1900, endtm.tm_mon + 1, endtm.tm_mday, endtm.tm_hour, endtm.tm_min, endtm.tm_sec,(pthread_t)pthread_self(), (pid_t)syscall(SYS_gettid));
	
	return 0;

}
