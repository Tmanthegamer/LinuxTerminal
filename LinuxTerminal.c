/* 
===============================================================================
SOURCE FILE:	LinuxTerminal.c 
					An application that will mimic the basic functions of the 
					standard linux terminal. 

PROGRAM:		Linux Terminal

FUNCTIONS:		int main(void)
				void ProcessInput(int p[])
				void ProcessOutput(int p[])
				void ProcessTranslate(int p[])
				char* TranslateRawInput(char* rawInput)
				void fatal(char* msg)
				

DATE:			January 9th, 2016

REVISIONS:

DESIGNGER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

NOTES:
THIS PROGRAM WILL REMOVE YOUR STANDARD LINUX TERMINAL FUNCTIONAL AND REPLACE IT WITH ITS OWN VERSION.

This program is designed to mimic the basic functions of writing to the 
terminal using the standard terminal keyboard in order to showcase the 
capability of processes and pipes.

What you will have access to via keyboard input: 
	-the Program Interrupt signal command ("ctrl-k" [that's the control button 
		and a lowercase k] )
	-standard Program termination command (pressing "T" [i.e. "shift-t"] )
	-the erase button (replaced with the "X" button)
	-line-kill which erases an entire line (pressing "K" [i.e. "shift-K"] )
===============================================================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "LinuxTerminal.h"
#include "Constants.h"


/*==== Main program entry point ====*/
int main(void)
{
	int translatePipe[2];
	int outputPipe[2];
	
	pid_t pid_translate;

	/* Open translate pipe communication. */
	if(pipe(translatePipe) < 0 || pipe(outputPipe) < 0){
		fatal("Bad pipe call, exiting...");
	}

	/* Set the O_NDELAY flag for p[0] */
	if (fcntl (translatePipe[0], F_SETFL, O_NDELAY) == -1 || 
		fcntl (outputPipe[0], F_SETFL, O_NDELAY) == -1){
		
		fatal ("Bad fcntl call, exiting...");
	}

	pid_translate = fork();
	switch(pid_translate)
  	{
    case -1:        /* error */
    	fatal ("Bad main fork call");
    	break;
    case 0:        /* It's the child */
    	display("Child Process: ProcessTranslate");
    	ProcessTranslate (translatePipe, outputPipe);
    	display("main: translation has left the building");
    	break;
    default:       /* parent */
    	display("Parent Process: ProcessInput");
    	ProcessInput (translatePipe);
    	wait(NULL);
    	display("main: input has left the building");
    	break;
  	}

  	return 0;
      
}

/*==== Assigned to the Input process to read keyboard input. ====*/
int ProcessInput(int translatePipe[])
{
	char confirm[BUFFERSIZE]= "ProcessInput: Sent Confirmed";
	char message[BUFFERSIZE];
	close(translatePipe[0]); /* Close the pipe for reading, don't need it. */
	
	while ((fgets(message, BUFFERSIZE, stdin)) != NULL) {
		display(confirm);
		write (translatePipe[1], message, BUFFERSIZE);
		
		/* If the termination character has been detected. */
		if(strstr(message, DEFAULT_EXIT) != 0){
			wait(NULL);
			break;
		}
	}

	display("End of ProcessInput");

	return 0;
}

/*==== Displays message to standard output ====*/
int ProcessOutput(int outputPipe[])
{
	int nread;
	char buf[BUFFERSIZE];

	close(outputPipe[1]); /* Close the pipe for writing, don't need it. */

	/*for (;;)-- DISABLED FOR NOW, TESTING REQUIRED --*/
	if(0)
	{
	    nread = read(outputPipe[0], buf, BUFFERSIZE);
	    switch (nread)
	    {
	    case -1:
	    case 0:
			printf ("(pipe empty)\n");
			sleep(1);
	    	break;
	    default:
	    	if (0) {
				printf ("Message received\n");
			  	exit(0);

		    } else {
		    	printf("Message received.\n");
		    	/*printf ("MSG = %s\n", buf);*/
		    }
				
	    } /* End of switch statement */

	} /* End of infinite for loop */

	return 0;
}

/*==== Receives raw input and sends the raw input and formatted data to ====
  ==== the Output. 														====*/
int ProcessTranslate(int inputPipe[], int outputPipe[])
{
	pid_t pid_output = 1;
	short quit = 0;
	int nread;
	char msg[BUFFERSIZE];
	char formatted[BUFFERSIZE];
	/*pid_output = fork();*/

	display("ProcessTranslate: Begin.");

	/* Disable writing to the input pipe, only reading required. */
	close(inputPipe[1]); 	
	
	/* Disable reading from the output pipe, only writing required. */
	/*close(outputPipe[0]);*/	

	switch(pid_output)
  	{
    case -1:        /* error */
    	fatal ("ProcessTranslate: Bad process translate fork call");
    	break;
    case 0:        /* It's the child */
    	display("ProcessTranslate: childerino");
    	/*ProcessOutput (outputPipe);*/
    	break;
    default:       /* parent */
    	{
    		display("ProcessTranslate: Infinite Read loop started.");
    		for(;;)
    		{
    			nread = read(inputPipe[0], msg, BUFFERSIZE);
    			switch (nread)
			    {
			    case -1:
			    case 0:
					sleep(1);
			    	break;
			    default:
			    	/*Display the Raw Message initially*/
			    	printf("Raw message: %s\n", msg);
			    	
					TranslateRawInput(msg, formatted);
					printf("Formatted:");
					display(formatted);
					printf("hit\n");
					/*
					write (outputPipe[1], message, BUFFERSIZE);
					*/
						
					/* Last part to section */
					if (strstr(msg, DEFAULT_EXIT) != 0) {
						display("ProcessTranslate: Ending ProcessTranslate");
						quit = 1;
				    }
			    } /* End of switch statement */

				if(quit){
					break;
				}
    		}/* End of for-loop */
    	}
    	break;
  	} /* End of switch statement */

  	return 0;

}

/*==== Translates the raw input and formats it. ====*/
void TranslateRawInput(const char* src, char* dest)
{
	int i, j = 0;

	for (i = 0; src[i] != '\0'; i++){
		if(src[i] == 'a'){
			dest[j++] = 'z';
		} else {
			dest[j++] = src[i];
		}
	}
	dest[j] = '\0';

}


/*==== Displays an error message and terminates the program immediately. ====*/
void fatal(char* errorMsg)
{
	perror(errorMsg);
	exit(1);
}

/*==== Prints the message to the standard output. ====*/
void display(const char* msg)
{
	printf("%s\n", msg);
}
