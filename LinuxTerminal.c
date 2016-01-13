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

/* ==== Global Variables ====*/
pid_t pid_translate, pid_output; // Used for immediate execution of processes.


/*==== Main program entry point ====*/
int main(void)
{
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
	
	struct sigaction sa;
	struct sigaction oldint;

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction (SIGINT, &sa, &oldint);
	sigaction (SIGTSTP, &sa, NULL);

	close(translatePipe[0]); /* Close the pipe for reading, don't need it. */
	
	while ((fgets(message, BUFFERSIZE, stdin)) != NULL) {
		display(confirm);
		if(FindCommand(message, IMMEDIATE_EXIT)){
			display("Kill Found!");
			kill(pid_translate, SIGKILL);
			kill(pid_output, SIGKILL);
			break;
		} 

		write (translatePipe[1], message, BUFFERSIZE);
		if(strstr(message, DEFAULT_EXIT)) {
			display("ProcessInput: Normal Termination.");
			wait(NULL);
			break;
		}

		
		
		/* If the termination character has been detected. */
		if(strstr(message, DEFAULT_EXIT) != 0){
			wait(NULL);
			break;
		}
	}
	sigaction (SIGINT, &oldint, NULL);

	display("Process Input: Ending ProcessInput");

	return 0;
}

/*==== Displays message to standard output ====*/
int ProcessOutput(int outputPipe[])
{
	short nread, quit = 0;
	char buf[BUFFERSIZE];

	close(outputPipe[1]); /* Close the pipe for writing, don't need it. */

	for (;;)
	{
	    nread = read(outputPipe[0], buf, BUFFERSIZE);
	    switch (nread)
	    {
	    case -1:
	    case 0:
			sleep(1);
	    	break;
	    default:
	    	if(strstr(buf, DEFAULT_EXIT) != 0){
	    		quit = 1;
	    	} 
			printf ("ProcessOutput: Message is:%s", buf);
	    } /* End of switch statement */

		//Check for normal termination request.
		if(quit){
			display("ProcessOutput: Normal Termination");
			break;
		}

	} /* End of infinite for loop */

	return 0;
}

/*==== Receives raw input and sends the raw input and formatted data to ====
  ==== the Output. 														====*/
int ProcessTranslate(int inputPipe[], int outputPipe[])
{
	short quit = 0;
	int nread;
	char msg[BUFFERSIZE];
	char formatted[BUFFERSIZE];
	

	pid_output = fork();

	display("ProcessTranslate: Begin.");

	/* Disable writing to the input pipe, only reading required. */
	close(inputPipe[1]); 	
	
	

	switch(pid_output)
  	{
    case -1:        /* error */
    	fatal ("ProcessTranslate: Bad process translate fork call");
    	break;
    case 0:        /* It's the child */
    	display("ProcessTranslate: Child, beginning ProcessOutput");
    	ProcessOutput (outputPipe);
    	break;
    default:       /* Perform the translation function. */
    	{
    		/* Disable reading from the output pipe, only writing required. */
			close(outputPipe[0]);	
    		
    		display("ProcessTranslate: Infinite Read loop started.");
    		for(;;)
    		{
    			nread = read(inputPipe[0], msg, BUFFERSIZE);
    			switch (nread)
			    {
			    case -1:
			    case 0:
			    	break;
			    default:
			    	/*Display the Raw Message initially*/
			    	write (outputPipe[1], msg, BUFFERSIZE);
			    	
			    	TranslateRawInput(msg, formatted);
			    	write (outputPipe[1], formatted, BUFFERSIZE);
			    	
			    	/* printf("Raw message: %s\n", msg);
					printf("Formatted:");
					display(formatted);*/
					
						
					/* Last part to section */
					if (strstr(msg, DEFAULT_EXIT) != 0) {
						display("ProcessTranslate: Ending ProcessTranslate");
						wait(NULL);
						quit = 1;
				    }
			    } /* End of switch statement */

				//Check for normal termination request.
				if(quit){
					display("ProcessTranslate: Normal Termination");
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
	if (sig == SIGKILL){
		printf ("got SIGINT\n");
	}
	else if(sig == SIGTERM){
		printf("not a signal");
	} else if(sig == SIGINT) {
		printf("naw");
	} else {
		printf("poop");
	}
}

/* Search for ctrl-k */
int FindCommand(const char* haystack, const int needle)
{
	int i;
	for(i = 0; i < BUFFERSIZE; i++){
		if(haystack[i] == needle){
			return 1;
		}
	}
	return 0;
}