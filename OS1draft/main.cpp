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

#include <signal.h>

#include <vector>

#include <chrono>
#include <thread>
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds


#define MAX_ARG 20
#define MAX_LINE_SIZE 80
#define MAXARGS 20
#define MAXHISTORY 50

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

time_t calcJobTime;

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
    
    int getSerial(){return serial_;}

    string getCommand(){return command_;}
    
    int getPid(){return pid_;}
    
    time_t getTime_(){return time_;}
    
    //true if stopped
    bool isStopped_(){ return stopped_;}
    
    void setStopped_(bool setStatus){
        if (setStatus == true){ //user want process to STOP!
            stopped_=true;
        }
        else{ //user want process to RUN!
            stopped_=false;
        }
    }
    
    void printJob(){
        time_t runTime = time(NULL) - time_;
        cout << "[" << serial_ << "] " << command_ << " : " << pid_ <<  " "<< runTime << " secs";
        if (stopped_==true)
            cout << " (Stopped)";
        cout << endl;
    }
    
};

vector<Job*> jobsVector;

Job* currentJob;

Job* findJob(int serial){
    for (std::vector<Job>::iterator it=jobsVector.begin(); it != jobsVector.end(); ++it){
        if (it->getSerial() == serial){
            //return *it;
            return NULL;
        }
    }
    return NULL;
}

void modifyJobList(){
    pid_t wait;
    int status;
    for (std::vector<Job>::iterator it=jobsVector.begin(); it != jobsVector.end(); ++it){
        wait = waitpid(it->getPid(), &status, WNOHANG | WUNTRACED);
        if (wait > 0){
            if ( WIFEXITED(status) || WIFSIGNALED(status) ) {
                jobsVector.erase(it);
            }
            else if (wait == -1){
                if (errno == ECHILD){
                  jobsVector.erase(it);
                }
                else
                    perror("wait has failed");
            }
            
        }
    }
}


/*
 *
 */

string whichSignal(int signalNumber){
    switch(signalNumber) {

        case 1 : return "SIGHUP"    ;
        case 2 : return "SIGINT"    ;
        case 3 : return "SIGQUIT"   ;
        case 4 : return "SIGILL"    ;
        case 5 : return "SIGTRAP"   ;
        case 6 : return "SIGABRT"   ;
        case 7 : return "SIGBUS"    ;
        case 8 : return "SIGFPE"    ;
        case 9 : return "SIGKILL"   ;
        case 10: return "SIGUSR1"   ;
        case 11: return "SIGSEGV"   ;
        case 12: return "SIGUSR2"   ;
        case 13: return "SIGPIPE"   ;
        case 14: return "SIGALRM"   ;
        case 15: return "SIGTERM"   ;
        case 16: return "SIGSTKFLT" ;
        case 17: return "SIGCHLD"   ;
        case 18: return "SIGCONT"   ;
        case 19: return "SIGSTOP"   ;
        case 20: return "SIGTSTP"   ;
        case 21: return "SIGTTIN"   ;
        case 22: return "SIGTTOU"   ;
        case 23: return "SIGURG"    ;
        case 24: return "SIGXCPU"   ;
        case 25: return "SIGXFSZ"   ;
        case 26: return "SIGVTALRM" ;
        case 27: return "SIGPROF"   ;
        case 28: return "SIGWINCH"  ;
        case 29: return "SIGIO"     ;
        case 30: return "SIGPWR"    ;
        case 31: return "SIGSYS"    ;
        default: return ""          ;
            
    }
}

int testBG(){
     if (!strcmp(cmd, "bg")){
         Job* currentJob = NULL;
         if (num_arg > 1){
             perror("Too many parameters!\n");
             return 1;
         }
         if (num_arg == 1){
             currentJob = findJob(stoi(args[1]));
             if (currentJob == NULL){
                 cerr << "smash error: > " << args[1] << " job does not exist" << endl;
                 return 1;
             }
             if (currentJob->isStopped_() == false){
                 cerr << "smash error: > " << args[1] << " job already runs in background" << endl;
                 return 1;
             }

             // send kill to check if stopped
             //cout << args[1] << endl;
             //return 0;
         }
         
         if (num_arg == 0){
             for (std::vector<Job>::reverse_iterator rit=jobsVector.rbegin(); rit != jobsVector.rend(); ++rit){
                 if (rit->isStopped_() == true){
                     currentJob = &(Job*)rit; //
                     break; //MUST exit this for loop
                 }
                 cerr << "smash error: > " << " no waiting jobs in background\n" << endl;
                 return 1;
             }
         }
         if (kill(currentJob->getPid(), SIGCONT) == -1){
             cerr << "smash error: > kill" << currentJob->getSerial() << "– cannot send signal\n" << endl;
             cerr << "erase this comment! job stopped but couldn't send cont signal" << endl;
             return 1;
         }
         else{
             currentJob->setStopped_(false);
             cout << currentJob->getCommand() << endl;
         }
         return 0;
     }
}
             


int testFG(){
     if (!strcmp(cmd, "fg")){
         Job* currentJob = NULL;
         if (num_arg > 1){
             perror("Too many parameters!\n");
             return 1;
         }
         if (num_arg == 1){
             currentJob = findJob(stoi(args[1]));
             if (currentJob == NULL){
                 cerr << "smash error: > " << args[1] << " job does not exist" << endl;
                 return 1;
             }
         }
         
         if (num_arg == 0){
             if ( jobsVector.size() == 0 ){
                 cerr << "smash error: > " << " job list is empty\n" << endl;
                 return 1;
             }
             else{
                 std::vector<Job>::reverse_iterator rit=jobsVector.rbegin();
                 currentJob = *rit;
                 //Job curry = *rit;
             }
         }
         //update the command to run in FG.   use the command pid to actually run it over smash
         
         
         if (kill(currentJob->getPid(), SIGCONT) == -1){
             cerr << "smash error: > kill" << currentJob->getSerial() << "– cannot send signal\n" << endl;
             cerr << "erase this comment! job stopped but couldn't send cont signal" << endl;
             return 1;
         }
         else{
             currentJob->setStopped_(false);
             cout << currentJob->getCommand() << endl;
         }
         return 0;
     }
}



int testFunc(){
     if (!strcmp(cmd, "func")){
         
     }
    return 0;
}



/*
 *
 */
int testKill(){
    if (!strcmp(cmd, "kill")){
        if (num_arg != 2){
            perror("Number of parameters doesn't match!\n");
            return 1;
        }
        Job* currentJob = findJob(stoi(args[2]));
        if (currentJob == NULL){
            cerr << "smash error: > kill" << args[2] << "– job does not exist" << endl;
            return 1;
        }
        
        int killNum = stoi(args[1]);
        if (killNum < 0)
            killNum = killNum*(-1);
        else {
            perror("illegal signal parameter!\n");
            return 1;
        }
        if ( killNum>31){
            perror("illegal signal parameter! bigger than 31\n");
            return 1;
        }
        if (kill(currentJob->getPid(), killNum) == -1){
            cerr << "smash error: > kill" << args[2] << "– cannot send signal" << endl;
            return 1;
        }
        if ( (killNum == SIGSTOP) || (killNum == SIGTSTP) ){
            currentJob->setStopped_(true);
        }
        else if (killNum == SIGCONT ){
            currentJob->setStopped_(false);
        }
        else if ( (killNum == SIGKILL) || (killNum == SIGTERM) ){
            modifyJobList();
        }

        
        for (std::list<string>::iterator it=cmdHistory.begin(); it != cmdHistory.end(); ++it)
          std::cout << *it << endl;
        return 0;
        }
    return 0;
}


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

int testJobs(){
    if (!strcmp(cmd, "jobs"))
    {
        if (num_arg != 0){
            perror("Too many parameters!\n");
            return 1;
        }
        //need to add modify jobs
        modifyJobList();
        for (std::vector<Job>::iterator it=jobsVector.begin(); it != jobsVector.end(); ++it)
            it->printJob();
    }
    
    return 0;
    
}


int Job::jobCount = 0;




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
    std::cout << realtime << endl;   //count from 1957
    
    
    
    cout << time(NULL) - realtime << endl;
    //Job(string command, int pid, bool stopped) : command_(command), pid_(pid), stopped_(stopped){
    //Job job1("cd .. &", 123, true);
    //Job job2("cd -", 127, false);
    //Job job3("pwd", 131, true);

    
    //jobsVector.push_back(job1);
    //jobsVector.push_back(job2);
    //jobsVector.push_back(job3);
    
    
    
    
    for (std::vector<Job>::iterator it=jobsVector.begin(); it != jobsVector.end(); ++it)
        it->printJob();
    int* d =(int*)malloc(sizeof(int));
 
    cout << "Test stoi : \n" << endl;
    
    
    string num = "5";
    string negative = "-1";
    int myint1 = stoi(num);
    cout << myint1 << endl;
    int myint2 = stoi(negative);
    cout << myint2 << endl;
    int killy;
    if (myint2 < 0)
        myint2 = myint2*(-1);
     cout << myint2 << endl;
    /*
    set(lineSize);
    testCd(args[0]); //cd ..
    set(makaf);
    testCd(args[0]); //cd -
    set(makafi);
    testCd(args[0]); //cd -
    */
    //cout << waitpid(getpid(), NULL, WNOHANG) << endl;
    
    //cout << "smash pid is " << getpid() << endl;
    

    return 0;
}

/*
 


  //put initialiser in h file


 //using std::cout;
 //using std::endl;
 //using std::string;
 */



/*
 //sleep_for(seconds(3)); //wait for 3
 
 cout << "1st job: " << endl;
 job1.printJob();
 cout << endl;
 
 //sleep_for(seconds(8)); //wait for 3
 
 cout << "2nd job: " << endl;
 job2.printJob();
 cout << endl;
 
 cout << "3rd job: " << endl;
 job3.printJob();
 cout << endl;
 cout << endl;
 cout << "formidable 123" << endl;
 cout << endl;
 
 */




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



