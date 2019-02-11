#include <stdio.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**/
/* Declaring a pointer of type file. Needed for communication between file and program*/
FILE *fptr;

int main()
{
	printf("This program is to test file funcions\n");
	
	/* fopen command opens the file of name specified. if no usch file exists it creates the file and opens it for writing as per the mode set "w" */
	fptr = fopen("testprog.txt", "w");
	
	 if(fptr == NULL)
   {
      printf("Error!");   
      return 0;             
   }
	
	/*Modify permissions of file to make it read write executable to everyone*/
	chmod("testprog.txt", 0777);
	
	/*
	char ch;	
	printf("Enter the character to write on the file..\n");
	scanf(" %c", &ch);
	fputc(ch,fptr)
	*/
	
	fputc('1', fptr);
    fclose(fptr);
    fptr = NULL;
	
	fptr = fopen("testprog.txt", "a");
	char * string = (char *)malloc (100 * sizeof(char));
	
	if(string == NULL)
	{
		printf("Null Pointer returned");
	return 0;
	}
	printf("Enter the string to write in the file \n");
	scanf("%[^\n]s", string);
	fputc('\n',fptr);
	fputs(string,fptr);
	fflush(fptr);
	fclose(fptr);
	fptr = NULL;
	
	fptr = fopen("testprog.txt", "r");
	
	 if(fptr == NULL)
   {
      printf("Error!");   
      return 0;             
   }
	else
	{
		char ch = fgetc(fptr);
		while(ch != EOF)
	{
		ch = fgetc(fptr);
		printf("%c",ch);

	}
	printf("\n");
		fclose(fptr);
	}
	free(string);

	
	
	
}

