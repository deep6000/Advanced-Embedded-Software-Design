#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#define BUF_LEN	(256)
#define SEED	(50)


#define __sys_mysort 		(398)



unsigned long my_merge_sort(const int32_t *ip_buffer, int buffer_len, int32_t *sortedBuffer)
{
	return syscall(__sys_mysort, ip_buffer, buffer_len, sortedBuffer);
         
}

void print_syscall_report( long status, int32_t *buffer )
{

	if(status == 0) 
	{
		printf("\n--------------SYSTEM CALL EXCECUTED PROPERLY--------------\n");
		printf("KERNEL LOG \n");
		system("dmesg | tail -9");
		printf("BUFFER AFTER SORT \n");
		for(int i = 0; i < BUF_LEN; i++)
		{
			printf("[%d] ",buffer[i]);
		}
		printf("\n");		
	}	
	else 
	{
		printf("\n----------------SYSTEM CALL DID NOT EXECUTE PROPERLY--------------\n Error:%ld:%s\n",status,strerror(status));
		system("dmesg | tail -9");

	}
}

int main()
{
	printf("PROCESS ID:\t %d\n", getpid());
	printf("USER ID:\t %u\n", getuid()); 
	system("date");    
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
		printf("{%d} ",Ip_Buf[i]);
	}

	printf("\n");
	int *Sort_Buf = (int32_t*)malloc(sizeof(int)*BUF_LEN);
	
	if(Sort_Buf == NULL)
	{
		printf("\n--------------ALLOCATION FAILURE--------------\n");
		free(Ip_Buf);
		return (-1);
	}


	printf("\n--------------OUTPUT BUFFER BEFORE SORTING--------------\n");
 		for(int i = 0; i < BUF_LEN; i++)
	{
		Sort_Buf[i] = 0;
		printf("{%d} ",Sort_Buf[i]);
	} 

	printf("\n");
	printf("\n--------------CALLING SYSTEM CALL MYSORT--------------\n");

	long sysexec_status = my_merge_sort(Ip_Buf,BUF_LEN,Sort_Buf);
	print_syscall_report( sysexec_status, Sort_Buf );
	printf("\n");

	printf("\n--------------CHECKING ERROR CONDITIONS--------------\n\n");
	
	
	printf("\n--------------SYSTEM CALL - NULL IN INPUT_BUFFER--------------\n");

	sysexec_status = my_merge_sort(NULL,BUF_LEN,Sort_Buf);
	print_syscall_report( sysexec_status, Sort_Buf );

	printf("\n");
	printf("\n--------------SYSTEM CALL - NULL IN OUTPUT_BUFFER--------------\n");

	sysexec_status = my_merge_sort(Ip_Buf,BUF_LEN,NULL);

	print_syscall_report( sysexec_status, Sort_Buf );



	printf("\n");

	printf("\n--------------SYSTEM CALL MIN LENGTH LIMIT NOT SATISFIED--------------");

	sysexec_status = my_merge_sort(Ip_Buf,10,Sort_Buf);
	print_syscall_report( sysexec_status, Sort_Buf );
	printf("\n");

	
	return (0);
}
