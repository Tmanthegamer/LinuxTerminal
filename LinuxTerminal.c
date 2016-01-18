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
#include "LinuxTerminal.h"

/*==== Main program entry point ====*/
int main(void)
{
	struct sigaction sa;
	struct sigaction oldint;

	int translatePipe[2];
	int outputPipe[2];

	system ("/bin/stty raw");

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
	char c;
	int i = 0;

	close(translatePipe[0]); /* Close the pipe for reading, don't need it. */
	
	while(!quit){
		c = getchar();
		printf("c:%c\n", c);
		switch(c)
		{
		/* Null character and newline restarts the while loop. */
		case '\0':
		case '\n':
			continue;

		/* 
		Acts like the enter key inserting a newline and null terminating
		the string.
		*/
		case 'E':
			message[i++] = c;
			message[i++] = '\n';
			message[i++] = '\0';
			write (translatePipe[1], message, BUFFERSIZE);
			
			i = 0;
			message[i] = '\0';
			break;
		case 'T':
			message[i++] = c;
			message[i++] = '\n';
			message[i++] = '\0';
			
			/* Sacrifices the children first before Process Input itself. */
			wait(NULL);
			quit = 1;
			break;

		case IMMEDIATE_EXIT:
			quit = 1;
			system ("stty -raw -igncr echo");
			kill(0, SIGKILL);
			break;

		default:
			message[i++] = c;
			break;	
		}
	}
	system ("stty -raw -igncr echo");

	#if 0
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
	#endif

	/*display("Process Input: Ending ProcessInput");*/

	return 0;
}

/*==== Displays message to standard output ====*/
int ProcessOutput(int outputPipe[])
{
	short nread;
	char buf[BUFFERSIZE]={'\0'};

	close(outputPipe[1]); /* Close the pipe for writing, don't need it. */

	while(!quit)
	{
	    nread = read(outputPipe[0], buf, BUFFERSIZE);
	    switch (nread)
	    {
	    case -1:
	    case 0:
			
	    	break;
	    default:
	    	printf ("%s", buf);
			fflush(stdout);

			if(strstr(buf, DEFAULT_EXIT) != 0){
	    		quit = 1;
	    	} 
	    } /* End of switch statement */

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
	
	char* raw 		= "Raw   : ";
	char* format 	= "Format: ";

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
    		
    		while(!quit)
    		{
    			nread = read(inputPipe[0], msg, BUFFERSIZE);
    			switch (nread)
			    {
			    case -1:
			    case 0:
			    	break;

			    default:
			    
			    	/*Display the Raw Message initially*/
			    	appendMessage(raw, msg, rawMessage);
			    	write (outputPipe[1], rawMessage, BUFFERSIZE);

			    	/*Translate the Formatted Message and display it.*/
			    	TranslateRawInput(msg, formatted);
			    	appendMessage(format, formatted, formattedMessage);

			    	write (outputPipe[1], formattedMessage, BUFFERSIZE);	
			    	

					/* Terminate when the DEFAULT_EXIT character is detected. */
					if (strstr(msg, DEFAULT_EXIT) != 0) {
						wait(NULL);
						quit = 1;
				    }
			    } /* End of switch statement */

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
		} else if(src[i] == 'X'){
			dest[j++] = '\b';
		} else if(src[i] == 'K'){
			while(j > -1){
				dest[j--] = '\0';
			}
			j = 0;
		} else {
			dest[j++] = src[i];
		}
	}
	dest[i] = '\0';

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
		if(i >= BUFFERSIZE-1)
			break;
		
		dest[i] = first[i];
	}

	for(j = 0; second[j] != '\0'; j++){
		
		/* Break if the buffersize has been reached */
		if(i >= BUFFERSIZE-1)
			break;

		/* append the second to the destination. */
		dest[i++] = second[j];
	}
	/* The final character will always be null terminated. */
	dest[i] = '\0';
}

