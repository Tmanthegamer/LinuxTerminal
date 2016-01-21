/* 
===========================================================================
SOURCE FILE:	Constants.h
					Header file that contains all the pre-defined 
					constants that will be used all throughout the Linux 
					Terminal application.

PROGRAM:		Linux Terminal

CONSTANTS:		BUFFERSIZE
					Size of a standard character array.

DATE:			January 9, 2016

REVISIONS:

DESIGNGER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

NOTES:
Contains all the global variables and definitions for ease of readability.
Descriptions of the purpose for each global variable is indicated by the 
variable declaration.
===========================================================================
*/

/* The available buffersize of all char arrays. Note: please don't sleep 
on the keyboard. This error is handled but your output will not be 
properly formatted and will look funny. */
#define BUFFERSIZE 4098

/* Signal sent when ctrl-k is sent. Only the input will search for this 
character. If the character is found, this program will abnormally close. 
*/
#define IMMEDIATE_EXIT 0x0B

/*  ==========================
	==== Global Variables ==== 
   	==========================  */

/* Signed integer representing the translate process ID. */ 
pid_t pid_translate;	

/* Signed integer representing the output process ID. */ 
pid_t pid_output;

/* Global integer used to softly terminate the Linux Terminal */ 	
static int quit = 0;

/* Global "macro" that all processes search for. 
		If found, finish all processing before terminating the program. */ 
static char* DEFAULT_EXIT = "T";
