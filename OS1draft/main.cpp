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

//using std::cout;
//using std::endl;
//using std::string;
using namespace std;

char* cmd;
char* args[MAX_ARG];

char* oldPwd = NULL; //assign * and nullptr
char* tmpPwd = NULL;



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
             if(!strcmp(args[1], dash)){ //enter if parameter is '-'
                 if (oldPwd == NULL){
                     perror("No such file or directory\n"); //do we need a \n
                     illegal_cmd = true;
                     return 1;
                 }
                 else{
                     strcpy(nextPwd, oldPwd); //oldPwd isn't NULL
                 }
                 
             } //finish if parameter is '-'
             else{
                 strcpy(nextPwd, args[1]);
             }

            strcpy(tmpPwd, getcwd(NULL, 0)); //tmpPwd <- current directory
            if(chdir(nextPwd) == 0){
                char* current = getcwd(NULL, 0);
                cout << current << endl;
                //strcpy(oldPwd, tmpPwd);
                
            }
            else{
                cerr << " \"" << nextPwd << "\" - No such file or directory\n" << endl;
            }

        }
    }
}

void showpid(){
    if (!strcmp(cmd, "showpid") )
    {
       if(num_arg > 0){
            perror("Too many parameters!\n");
            illegal_cmd = true;
        }
       else{
           cout << "smash pid is " << getpid() << endl;
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
    //testCd(args[0]); //cd ..
    
    set(makaf);
    //testCd(args[0]); //cd -
   
    
    cout << "smash pid is " << getpid() << endl;
    while(1){
        perror("No such file or directory\n");
    }
    cout << " ya tz a ti" << endl;
    
    
    ///
    //char check[MAX_LINE_SIZE];
    //cout << strlen(check) << endl;
    
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
