/* 
===============================================================================
SOURCE FILE:	LinuxTerminal.h
					An application that will mimic the basic 
					functions of the standard linux terminal. 

PROGRAM:		Linux Terminal

FUNCTIONS:		int main(void)
				int ProcessInput(int p[])
				int ProcessOutput(int p[])
				int ProcessTranslate(int p[])
				void fatal(char* msg)
				char* TranslateRawInput(char* rawInput)

DATE:			January 9th, 2016

REVISIONS:

DESIGNGER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

NOTES:
This is a standard C program which will read characters from a terminal 
keyboard and will echo those characters to the terminal screen while 
processing each line of the characters. 

The purpose of this program is to show off the functionality of processes and 
using pipes to pass information between those processes. There are three 
processes:

-Input: 	Reads each raw character entered at the keyboard into a buffer and 
			sends the "line" of characters to Output and Translate when the 
			ENTER key is pressed.
-Translate:	Processes the raw characters received from Input. This process 
			will perform the normal Linux erase(X), line-kill(K), 
			interupt (ctrl-k) and normal termination of the program(T). In 
			addition, it will convert all 'a' characters in the string to the 
			'z' character. The newly converted output will be sent to the 
			Output process.
-Output:	Simple process that will receive characters from 'Input' and 
			Translate and display them to the terminal screen.


*IMPORTANT* PLEASE NOTE, THIS PROGRAM WILL REMOVE YOUR STANDARD	LINUX 
TERMINAL FUNCTIONALITY AND REPLACE IT WITH ITS OWN VERSION. MEANING THAT YOUR STANDARD CTRL-C OR CTRL-D WILL NOT WORK.
===============================================================================
*/


/*
===============================================================================
FUNCTION: 		Main 

DATE:			January 9, 2016

REVISIONS:		(Date and Description)

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER(S):	Tyler Trepanier-Bracken

INTERFACE:		int main (void)

PARAMETERS:		void

RETURNS:		int:	

NOTES:
Main entry point into the program. This function will: 

	(1) Initially creates the pipes which will allow all the processes to 
		communicate with each other. 
	(2) Afterwards it creates the three processes: Input, Translate and Output.
	(3)	Assigns the Input process the ProcessInput task.
	(4)	Assigns the Translate process the ProcessTranslate task.
	(5)	Assigns the Output process the ProcessOutput task.
===============================================================================
*/
int main(void);

/*
===============================================================================
FUNCTION: 		ProcessInput 

DATE:			January 9, 2016

REVISIONS:		(Date and Description)

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER(S):	Tyler Trepanier-Bracken

INTERFACE:		int ProcessInput(int p[])

PARAMETERS:		int p[] 
					Pipes that will be used for communication between the 
					Input and Translate.

RETURNS:		-Returns -1 on abnormal process termination of program 
					(Pressing "ctrl-k")    
				-Returns 0 on normal termination of program
					(Pressing "T").

NOTES:
This function is assigned to the Input process where its sole purpose is to 
constantly read keyboard input. When the ENTER key is pressed, all of the RAW 
input will sent to the translate function where it will be processed. 

Pressing 'T'(that's a captial 't') results in a normal termination of the 
program which will output the raw input to the Translate process. The 
Translate and Output processes will be closed off.

Pressing 'ctrl-k' (that's a control button and a lowercase 'k') immediately 
suspends the program, bypassing the regular translate process function and 
force closing the Translate and Output process.

Reading from the pipe will be closed off for this function because it is 
unnecessary for this function's purpose.
===============================================================================
*/
int ProcessInput(int p[]);

/*
===============================================================================
FUNCTION: 		ProcessOutput 

DATE:			January 9, 2016

REVISIONS:		(Date and Description)

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER(S):	Tyler Trepanier-Bracken

INTERFACE:		int ProcessOutput(int p[])

PARAMETERS:		int p[] 
					Pipes that will be used for communication between the 
					Output and Translate.

RETURNS:		-Returns -1 on abnormal process termination of program 
					(Pressing "ctrl-k")    
				-Returns 0 on normal termination of program
					(Pressing "T").

NOTES:
This function will be assigned to Output Process; this will display all 
messages to the standard output using the Pipes donated by the parent process.

Writing to the pipe will be closed off for this process because it is 
unrequired to its function.
===============================================================================
*/
int ProcessOutput(int p[]);

/*
===============================================================================
FUNCTION: 		ProcessTranslate 

DATE:			January 9, 2016

REVISIONS:		(Date and Description)

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER(S):	Tyler Trepanier-Bracken

INTERFACE:		int ProcessTranslate(int inputPipe[], 
									 int outputPipe[])

PARAMETERS:		int inputPipe[] 
					Pipe used to communicate with the Input Process
				int outputPipe[]
					Pipe used to communicate with the Output Process 

RETURNS:		-Returns -1 on abnormal process termination of program 
					(Pressing "ctrl-k")    
				-Returns 0 on normal termination of program
					(Pressing "T").

NOTES:
Associated with the Translate Process, this function reads the input gathered from the Input process via the pipe parameter.

(1) First it will send to the output the raw input received by the Input.
(2) Next it will check the string for any macros from the input. The macros will be removed from the line of input and will dynamically adjust the input until the end of the line.
(3) The result will be sent to the Output via the pipe again.
===============================================================================
*/
int ProcessTranslate(int inputPipe[], int outputPipe[]);

/*
===============================================================================
FUNCTION: 		TranslateRawInput 

DATE:			January 9, 2016

REVISIONS:		(Date and Description)

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER(S):	Tyler Trepanier-Bracken

INTERFACE:		int TranslateRawInput(int inputPipe[], 
										int outputPipe[])

PARAMETERS:		int outputPipe[]
					Pipe used to communicate with the Output Process 
				const char* rawMsg
					Raw message to be reformatted.

RETURNS:		char*
					Formatted message to be transferred to the Output.

NOTES:
Utility function used by the ProcessTranslate that will convert all a's into z's. 
===============================================================================
*/
int TranslateRawInput(int inputPipe[], int outputPipe[]);

/*
===============================================================================
FUNCTION: 		Fatal 

DATE:			January 9, 2016

REVISIONS:		(Date and Description)

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER(S):	Tyler Trepanier-Bracken

INTERFACE:		void fatal(char* errorMsg)

PARAMETERS:		char *errorMsg: 
					Message to be displayed to the standard output.

RETURNS:		-Returns -1 on abnormal process termination of program 
					(Pressing "ctrl-k")    
				-Returns 0 on normal termination of program
					(Pressing "T").

NOTES:
Displays an error message to the standard output and proceeds to terminate the program and all the processes.
===============================================================================
*/
void fatal(char* errorMsg);

/*
===============================================================================
FUNCTION: 		Fatal 

DATE:			January 10, 2016

REVISIONS:		(Date and Description)

DESIGNER:		Tyler Trepanier-Bracken

PROGRAMMER(S):	Tyler Trepanier-Bracken

INTERFACE:		void display(char* errorMsg)

PARAMETERS:		char *msg: 
					Message to be displayed to the standard output.

RETURNS:		void

NOTES:
Displays an message to the standard output and appends with a newline.
===============================================================================
*/
void display(char* msg);