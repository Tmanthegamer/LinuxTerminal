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
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "LinuxTerminal.h"
#include "Constants.h"

/*==== Main program entry point ====*/
int main(void)
{
	struct sigaction sa;
	struct sigaction oldint;

	int translatePipe[2];
	int outputPipe[2];

	/* Open translate pipe communication. */
	if(pipe(translatePipe) < 0 || pipe(outputPipe) < 0){
		fatal("Bad pipe call, exiting...");
	}

	/* Set the O_NDELAY flag for p[0] */
	if (fcntl (translatePipe[0], F_SETFL, O_NDELAY) == -1 || 
		fcntl (outputPipe[0], F_SETFL, O_NDELAY) == -1){
		
		fatal ("Bad fcntl call, exiting...");
	}

	sa.sa_handler = sig_handler;
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction (SIGINT, &sa, &oldint);
	sigaction (SIGTSTP, &sa, NULL);

	pid_translate = fork();
	switch(pid_translate)
  	{
    case -1:        /* error */
    	fatal ("Bad main fork call");
    	break;
    case 0:        /* It's the child */
    	ProcessTranslate (translatePipe, outputPipe);
    	break;
    default:       /* parent */
    	ProcessInput (translatePipe);
    	wait(NULL);
    	break;
  	}

  	sigaction (SIGINT, &oldint, NULL);

  	return 0;
      
}

/*==== Assigned to the Input process to read keyboard input. ====*/
int ProcessInput(int translatePipe[])
{
	/*char confirm[BUFFERSIZE]= "ProcessInput: Sent Confirmed";*/
	char message[BUFFERSIZE]={'\0'};

	close(translatePipe[0]); /* Close the pipe for reading, don't need it. */
	
	while ((fgets(message, BUFFERSIZE, stdin)) != NULL || !quit) {
		if(FindCommand(message)>= 0){
			kill(0, SIGKILL);
			break;
		}
		
		write (translatePipe[1], message, BUFFERSIZE);
		if(strstr(message, DEFAULT_EXIT)) {
			/*display("ProcessInput: Normal Termination.");*/

			wait(NULL);
			break;
		}
		
	}

	/*display("Process Input: Ending ProcessInput");*/

	return 0;
}

/*==== Displays message to standard output ====*/
int ProcessOutput(int outputPipe[])
{
	short nread;
	char buf[BUFFERSIZE]={'\0'};

	close(outputPipe[1]); /* Close the pipe for writing, don't need it. */

	for (;;)
	{
	    nread = read(outputPipe[0], buf, BUFFERSIZE);
	    switch (nread)
	    {
	    case -1:
	    case 0:
			
	    	break;
	    default:
	    	if(strstr(buf, DEFAULT_EXIT) != 0){
	    		quit = 1;
	    	} 
			printf ("%s", buf);
	    } /* End of switch statement */

		//Check for normal termination request.
		if(quit){
			/*display("ProcessOutput: Normal Termination");*/
			break;
		}

	} /* End of infinite for loop */

	return 0;
}

/*==== Receives raw input and sends the raw input and formatted data to ====
  ==== the Output. 														====*/
int ProcessTranslate(int inputPipe[], int outputPipe[])
{
	int nread;
	char msg[BUFFERSIZE]={'\0'};
	char formatted[BUFFERSIZE]={'\0'};

	char rawMessage[BUFFERSIZE] = {'\0'};
	char formattedMessage[BUFFERSIZE] = {'\0'};
	
	char* raw = "Raw: ";
	char* format = "Formatted: ";

	pid_output = fork();

	/* Disable writing to the input pipe, only reading required. */
	close(inputPipe[1]); 	
	
	

	switch(pid_output)
  	{
    case -1:        /* error */
    	fatal ("ProcessTranslate: Bad process translate fork call");
    	break;
    case 0:        /* It's the child */
    	/*display("ProcessTranslate: Child, beginning ProcessOutput");*/
    	ProcessOutput (outputPipe);
    	break;
    default:       /* Perform the translation function. */
    	{
    		/* Disable reading from the output pipe, only writing required. */
			close(outputPipe[0]);	
    		
    		for(;;)
    		{
    			nread = read(inputPipe[0], msg, BUFFERSIZE);
    			switch (nread)
			    {
			    case -1:
			    case 0:
			    	break;
			    default:
			    
			    	appendMessage(raw, msg, rawMessage);
			    	write (outputPipe[1], rawMessage, BUFFERSIZE);

			    	TranslateRawInput(msg, formatted);
			    	appendMessage(format, msg, formattedMessage);

			    	write (outputPipe[1], formattedMessage, BUFFERSIZE);	
			    	/*Display the Raw Message initially*/

					/* Last part to section */
					if (strstr(msg, DEFAULT_EXIT) != 0) {
						wait(NULL);
						quit = 1;
				    }
			    } /* End of switch statement */

				//Check for normal termination request.
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
void fatal(const char* errorMsg)
{
	perror(errorMsg);
	exit(1);
}

/*==== Prints the message to the standard output. ====*/
void display(const char* msg)
{
	printf("%s\n", msg);
}

/* Simple signal handler */
void sig_handler(int sig)
{
	/* Does absolutely nothing but ignore signal commands */
}

/* Search for ctrl-k */
int FindCommand(const char* haystack)
{
	int i;
	for(i = 0; i < BUFFERSIZE; i++){
		if(haystack[i] == IMMEDIATE_EXIT){
			return i;
		}
	}
	return -1;
}

/* Combine two strings together */
void appendMessage(const char* first, const char* second, char* dest)
{
	int i, j;
	for(i = 0; first[i] != '\0'; i++){
		
		/* Break if the buffersize has been reached */
		if(i == BUFFERSIZE)
			break;
		
		dest[i] = first[i];
	}

	for(j = 0; second[j] != '\0'; j++){
		
		/* Break if the buffersize has been reached */
		if(i == BUFFERSIZE)
			break;

		/* append the second to the destination. */
		dest[i++] = second[j];
	}
}