#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARG 20
#define MAX_LINE_SIZE 80
#define MAXARGS 20

using std::cout;
using std::endl;

char* cmd;
char* args[MAX_ARG];

char oldPwd[MAX_LINE_SIZE];
char tmpPwd[MAX_LINE_SIZE];

bool illegal_cmd = false; // illegal command

const char* delimiters = " \t\n";

int i = 0;
int num_arg = 0;


void testPWD(char * cmd){
    if (!strcmp(cmd, "pwd")){
        if (num_arg != 0){
            perror("Too many parameters!\n");
            illegal_cmd = true;
        }
        else{
            char* current = getcwd(NULL, 0);
            if (current == NULL){
                perror("falied to retreive current working directory\n");
                illegal_cmd = true;
            }

            else{
               cout << current << endl;
            }
        }
        
    }
}



void testCd(char * cmd){
    if (!strcmp(cmd, "cd") )
    {
        char dash[] = "-";
        if (num_arg == 0){
            perror("Not enough parameters!\n");
            illegal_cmd = true;
        }
        else if(num_arg > 1){
            perror("Too many parameters!\n");
            illegal_cmd = true;
        }
        
        //0 if equal  //we change folder   cd -
        else{
            char nextPwd[MAX_LINE_SIZE];
             if(!strcmp(args[1], dash)){
                 strcpy(nextPwd, oldPwd);
             }
             else{
                 strcpy(nextPwd, args[1]);
             }
            strcpy(tmpPwd, getcwd(NULL, 0));
            if(chdir(nextPwd) == 0){
                char* current = getcwd(NULL, 0);
                cout << current << endl;
                strcpy(oldPwd, tmpPwd);
            }

        }
    }
}

void set(char* lineSize){

    cmd = strtok(lineSize, delimiters);
    args[0] = cmd;
    num_arg=0;
    for (i=1; i<MAX_ARG; i++)
    {
        args[i] = strtok(NULL, delimiters);
        if (args[i] != NULL)
            num_arg++;
    
    }
}



int main(int argc, const char * argv[]) {
   
    char lineSize[] = "cd ..";
    char makaf[] = "cd -";
    
    set(lineSize);
    testCd(args[0]); //cd ..
    
    set(makaf);
    testCd(args[0]); //cd -
    


    return 0;
}


/*
 //testPWD(lineSize);
 char* current1 = getcwd(NULL, 0);
 char changeme[] = "..";
 char change[] = "cd ..";
 
 
 
 char* current2 = getcwd(NULL, 0);
 
 chdir(dash);
 
 char* current3 = getcwd(NULL, 0);
 
 cout << current1 << endl;
 cout << current2 << endl;
 cout << current3 << endl;
 
 
 */
