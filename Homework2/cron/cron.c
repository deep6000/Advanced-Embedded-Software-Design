#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

#define BUF_LEN	(256)
#define SEED	(50)


#define __sys_mysort 		(398)

unsigned long my_merge_sort(const int32_t *ip_buffer, int buffer_len, int32_t *sortedBuffer)
{
	return syscall(__sys_mysort, ip_buffer, buffer_len, sortedBuffer);
         
}

int sortsyscall()
{

 	clock_t t;
	srand(SEED);

	int *Ip_Buf = (int32_t*)malloc(sizeof(int)*BUF_LEN);
	if(Ip_Buf == NULL)
	{
		printf("\n--------------ALLOCATION FAILURE--------------\n");
		return (-1);
	}
	printf("\n--------------INPUT BUFFER BEFORE SORTING--------------.\n");
	for(int i = 0; i < BUF_LEN; i++)
	{
		Ip_Buf[i] = (rand()%50);
		printf("Element Number %d is {%d} \n",i,Ip_Buf[i]);
	}

	int *Sort_Buf = (int32_t*)malloc(sizeof(int)*BUF_LEN);
	if(Sort_Buf == NULL)
	{
		printf("\n--------------ALLOCATION FAILURE--------------\n");
		free(Ip_Buf);
		return (-1);
	}

	 t = clock();
	
	long sysexec_status = my_merge_sort(Ip_Buf,BUF_LEN,Sort_Buf);
	
	printf("\n--------------OUTPUT BUFFER AFTER SORTING--------------.\n");
	for(int i = 0; i < BUF_LEN; i++)
	{
		printf("Sorted list Element no %d is {%d} \n",i,Sort_Buf[i]);
	}

	t = (clock() - t);

	printf("Total time taken for system call -> %f\n", ((double)t/CLOCKS_PER_SEC));
	if (sysexec_status == 0)
	{
	printf("System call executed properly/n");	
	}
	else 
	printf("Error executing system call/n");
	
}

int main()
{
	printf("--------------------------------------------------------------------------------------------------------/n/n");
	printf("PROCESS ID:\t %d\n", getpid());
	printf("USER ID:\t %u\n", getuid()); 
	system("date");
	PRINT_PRESENT_TIME();
	sortsyscall();
	return 0;
}	

	
	
