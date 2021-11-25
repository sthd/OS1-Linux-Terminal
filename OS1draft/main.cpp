#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <list>
#include <time.h>

#define MAX_ARG 20
#define MAX_LINE_SIZE 80
#define MAXARGS 20
#define MAXHISTORY 50


 //put initialiser in h file


//using std::cout;
//using std::endl;
//using std::string;
using namespace std;

char* cmd;
char* args[MAX_ARG];

//char* oldPwd = NULL; //assign * and nullptr
//char* tmpPwd = NULL;
// You can initialize it the way your instructor suggested as you declare the array:
char nextPwd[MAX_LINE_SIZE] ="";
char oldPwd[MAX_LINE_SIZE] ="";
char tmpPwd[MAX_LINE_SIZE] ="";
char dash[] = "-";

list<string> cmdHistory;

bool illegal_cmd = false; // illegal command

const char* delimiters = " \t\n";

int i = 0;
int num_arg = 0;


class Job{
    int serial_;
    string command_;
    int pid_;
    time_t time_;
    bool stopped_;
    
    public:
    static int jobCount;
    Job(string command, int pid, bool stopped) : command_(command), pid_(pid), stopped_(stopped){
        serial_ =jobCount++;
        time_=(time(NULL));
    }
    
    int getSerial(){
        return serial_;
    }

    string getCommand(){
        return command_;
    }
    
    int getPid(){
        return pid_;
    }
    
    time_t getTime_(){
        return time_;
    }
    
    //true if stopped
    bool isStopped_(){
        return stopped_;
    }
    
    void setStopped_(bool setStatus){
        if (setStatus == true){ //user want process to STOP!
            stopped_=true;
        }
        else{ //user want process to RUN!
            stopped_=false;
        }
    }
    
    
};

int Job::jobCount = 0;

int testHistory(char * cmd){
    if (!strcmp(cmd, "history")){
        if (num_arg != 0){
            perror("Too many parameters!\n");
            return 1;
        }
        for (std::list<string>::iterator it=cmdHistory.begin(); it != cmdHistory.end(); ++it)
          std::cout << *it << endl;
        return 0;
        }
    return 0;
}



int testPWD(char * cmd){
    if (!strcmp(cmd, "pwd")){
        if (num_arg != 0){
            perror("Too many parameters!\n");
            return 1;
        }
        
        char* current = getcwd(NULL, 0);
        if (current == NULL){
            perror("falied to retreive current working directory\n");
            return 1;
        }
        cout << current << endl;
        return 0;
        }
    return 0;
}

    
int testCd(char * cmd){
    if (!strcmp(cmd, "cd") ){
        if (num_arg == 0){
            perror("Not enough parameters!\n");
            return 1;
        }
        else if(num_arg > 1){
            perror("Too many parameters!\n");
            return 1;
        }
        if(!strcmp(args[1], dash)){  //enter if parameter is '-'
            if (strlen(oldPwd) == 0){
                perror("No such file or directory\n"); //do we need a \n
                return 1;
            }
            else{
                strcpy(nextPwd, oldPwd); //oldPwd isn't NULL
            }
        } //finish if parameter is '-'
        else{
            strcpy(nextPwd, args[1]);
        }
        //nextPwd is either oldPwd or args[1]
        
        strcpy(tmpPwd, getcwd(NULL, 0)); //tmpPwd <- current directory
        if(chdir(nextPwd) == -1){
            cerr << " \"" << nextPwd << "\" - No such file or directory\n" << endl;
            return 1;
        }
        char* current = getcwd(NULL, 0);
        cout << current << endl;
        strcpy(oldPwd, tmpPwd); //enter last cwd into oldPwd
        return 0;
    }
    return 0;
}
    
    
int showpid(){
    if (!strcmp(cmd, "showpid") ){
       if(num_arg > 0){
           perror("Too many parameters!\n");
           return 1;
        }
        cout << "smash pid is " << getpid() << endl;
        return 0;
    }
    return 0;
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
   
    //cout << testPWD("pwd") << endl;
    
    char lineSize[] = "cd ../../Intermediates.noindex/XCBuildData";
    char makaf[] = "cd -";
    char makafi[] = "cd -";
    

    if (cmdHistory.size() == MAXHISTORY ){
        cmdHistory.pop_front();
    }
    cmdHistory.push_back("shalom");
    
    for (std::list<string>::iterator it=cmdHistory.begin(); it != cmdHistory.end(); ++it)
      std::cout << *it << endl;
    
    time_t realtime = time(NULL);
    std::cout << realtime << endl;
    
    
    
    /*
    set(lineSize);
    testCd(args[0]); //cd ..
    set(makaf);
    testCd(args[0]); //cd -
    set(makafi);
    testCd(args[0]); //cd -
    */
    
    
    //cout << "smash pid is " << getpid() << endl;
    

    return 0;
}



/*
 string myints[] = {"a", "b", "c","d","e"};
 std::list<string> cmdHistory; // (myints,myints+5);
 */


/*
 char check[MAX_LINE_SIZE] ="";
 cout << strlen(check) << endl;
 strcpy(check, lineSize);
 cout << check << endl;
 cout << strlen(check) << endl;
 cout << lineSize << endl;
 cout << strlen(lineSize) << endl;
 */















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
