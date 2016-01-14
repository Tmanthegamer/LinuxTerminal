/* 
===============================================================================
SOURCE FILE:	Constants.c 
					Header file that contains all the pre-defined constants that will be used all throughout the Linux Terminal application.

PROGRAM:		Linux Terminal

CONSTANTS:		BUFFERSIZE
					Size of a standard character array.

DATE:			January 9, 2016

REVISIONS:

DESIGNGER:		Tyler Trepanier-Bracken

PROGRAMMER:		Tyler Trepanier-Bracken

NOTES:
Standard Notes go here.
===============================================================================
*/
#define BUFFERSIZE 2048
#define IMMEDIATE_EXIT 0x0B
/* ==== Global Variables ====*/
pid_t pid_translate, pid_output; // Used for immediate execution of processes.
int quit = 0;
const char* DEFAULT_EXIT = "T";
