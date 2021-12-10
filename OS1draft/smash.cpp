/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "commands.h"
#include "signals.h"
#include "job.h"

//#include <string.h>


#define MAX_LINE_SIZE 80
#define MAXARGS 20


//char* L_Fg_Cmd;
//void* jobs = NULL; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
char lineSize[MAX_LINE_SIZE];

vector<Job> jobsVector;

int fg_pid;
string fg_cmd;

Job* currentJob;


void catch_int(int sig_num) {


}
int Daddy;
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[]){
    Daddy = getpid();
    //cout << Daddy << endl;
    //struct sigaction new_action, old_action;

    /* Set up the structure to specify the new action. */
    //new_action.sa_handler = termination_handler;
    //L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
    //char L_Fg_Cmd[MAX_LINE_SIZE+1];
    //if (L_Fg_Cmd == NULL)
    //        exit (-1);
    //L_Fg_Cmd[0] = '\0';

    
    //struct sigaction act;
    //act.sa_handler = &catch_int;
    //sigaction(SIGINT, &act, NULL);
    struct sigaction actTSTP;
    struct sigaction actINT;
    

    
    sigemptyset (&actTSTP.sa_mask);
    actTSTP.sa_flags = 0;
    sigemptyset (&actINT.sa_mask);
    actINT.sa_flags = 0;
    
    actTSTP.sa_handler = &ctrl_Z_handler;
    actINT.sa_handler = &ctrl_C_handler;
    
    sigaction(SIGTSTP, &actTSTP, NULL);
    sigaction(SIGINT, &actINT, NULL); //ctrlC
    char cmdString[MAX_LINE_SIZE]; 	   
	

	
    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin); //lineSize is our variable
		strcpy(cmdString, lineSize);
		cmdString[strlen(lineSize)-1]='\0';
            
        // perform a complicated Command
		if(!ExeComp(lineSize)) continue;
            
        // background command
	 	if(!BgCmd(lineSize)) continue;
            cout << lineSize << "here" << endl;
        // built in commands
		ExeCmd(lineSize, cmdString);
            
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
            
            

        
	}
    //free(L_Fg_Cmd);
    return 0;
}







//signal declaretions
//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
 /* add your code here */

/************************************/
//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
//set your signal handlers here
/* add your code here */

/************************************/

/************************************/
// Init globals
