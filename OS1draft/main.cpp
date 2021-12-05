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

#include <fcntl.h>

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

int fg_pid;
string fg_cmd;

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
    
    
    bool isStopped_(){ return stopped_;} //returns TRUE if job STOPPED
    
    
    void setStopped_(bool setStatus){
        if (setStatus == true) stopped_=true; //set job as STOPPED!
        else stopped_=false; //set job as RUNNING!
    }
    
    void printJob(){
        time_t runTime = time(NULL) - time_;
        cout << "[" << serial_ << "] " << command_ << " : " << pid_ <<  " "<< runTime << " secs";
        if (stopped_==true)
            cout << " (Stopped)";
        cout << endl;
    }
    
};

vector<Job> jobsVector;

Job* currentJob;

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

Job* findJobSerial(int serial){
    //modifyJobList();
    for (std::vector<Job>::iterator it=jobsVector.begin(); it != jobsVector.end(); ++it){
        if (it->getSerial() == serial){
            return &(*it);
        }
    }
    return NULL;
}

Job* findJobPID(int pid){
    //modifyJobList();
    for (std::vector<Job>::iterator it=jobsVector.begin(); it != jobsVector.end(); ++it){
        if (it->getPid() == pid){
            return &(*it);
        }
    }
    return NULL;
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
         modifyJobList();
         if (num_arg == 1){
             currentJob = findJobSerial(stoi(args[1]));
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
                     currentJob = &(*rit); //
                     break; //MUST exit this for loop
                 }
             }
             if (currentJob==NULL){
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
    return 0;
}
             


int testFG(){
     if (!strcmp(cmd, "fg")){
         Job* currentJob = NULL;
         if (num_arg > 1){
             perror("Too many parameters!\n");
             return 1;
         }
         modifyJobList();
         if (num_arg == 1){
             currentJob = findJobSerial(stoi(args[1]));
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
                 currentJob = &(*rit);
             }
         }
         //update the command to run in FG.
         //use the command pid to actually run it over smash
         
         if(currentJob->isStopped_() == true){
             if (kill(currentJob->getPid(), SIGCONT) == -1){
                 cerr << "smash error: > kill" << currentJob->getSerial() << "– cannot send signal\n" << endl;
                 cerr << "erase this comment! job stopped but couldn't send cont signal" << endl;
                 return 1;
             }
         }
         currentJob->setStopped_(false);
         fg_pid=currentJob->getPid();
         fg_cmd = currentJob->getCommand();  //consider char* or strcpy etc
         cout << currentJob->getCommand() << endl;
         if (waitpid(fg_pid, NULL, WUNTRACED) ==-1){
             //assigned NULL for status so we can run
              cerr << "waitpid for FG failed" << endl;
             return 1;
         }
         fg_pid=0;
         fg_cmd = "SMASH";
         return 0;
         // !!CHECK: need to print sig cont etc
     }
    return 0;
}



int testFunc(){
     if (!strcmp(cmd, "func")){
         
     }
    return 0;
}

int testDiff(){
     if (!strcmp(cmd, "diff")){
         int file1, file2;
         ssize_t bytesRead1, bytesRead2;
         const int nbytes=1;
         
         file1 = open(args[1], O_RDONLY);
         if (file1 == -1){
             cerr << "FILE1 ERROR: No such file or directory" << endl;
             return 1;
         }
         file2 = open(args[2], O_RDONLY);
         if (file2 == -1){
             if (close(file1) == -1){
                cerr << "FILE1 ERROR: close file failed!" << endl;
             }
             cerr << "FILE2 ERROR: No such file or directory" << endl;
             return 1;
         }
         
         char* buff1 = NULL;
         char* buff2 = NULL;
         bytesRead1 = read(file1, buff1, nbytes);
         bytesRead2 = read(file2, buff2, nbytes);
         while(bytesRead1!=0 || bytesRead2!=0){
             if(bytesRead1==-1 || bytesRead2==-1){
                cerr << "Unable to read file" << endl;
                 if (close(file1) == -1 || close(file2) == -1  ){
                     cerr << "FILE ERROR: close file failed!" << endl;
                     return 1;
                 }
             }
             if (bytesRead1==0 || bytesRead2==0){
                 cout << "1" <<endl;
                 if (close(file1) == -1 || close(file2) == -1  ){
                     cerr << "FILE ERROR: close file failed!" << endl;
                     return 1;
                 }
                 return 0;
             }
             if (*buff1!=*buff2){
                 cout << "1" <<endl;
                     if (close(file1) == -1 || close(file2) == -1  ){
                         cerr << "FILE ERROR: close file failed!" << endl;
                         return 1;
                     }
                     return 0;
            }
             bytesRead1 = read(file1, buff1, nbytes);
             bytesRead2 = read(file2, buff2, nbytes);
             
         }
         cout << "0" <<endl;
         if (close(file1) == -1 || close(file2) == -1  ){
            cerr << "FILE ERROR: close file failed!" << endl;
            return 1;
        }
     }
    return 0;
}

int testQuit(){
     if (!strcmp(cmd, "quit")){
         if (num_arg > 1){
            cerr << "smash error: > too many argument for quit" << endl;
            return 1;
         }
         if (num_arg == 0){
            exit(0);
         }
         //if args[1] == kill we return 1
         modifyJobList();
         if (num_arg ==1 && (!strcmp(args[1], "kill") ) ){
            //go through jobs
            //send sigterm to each job
            //check if job terminated
             for (std::vector<Job>::iterator it=jobsVector.begin(); it != jobsVector.end(); ++it){
                 if (kill(it->getPid(), SIGTERM) == -1){
                     cerr << "smash error: > kill" << it->getSerial() << "– cannot send signal\n" << endl;
                     return 1; //system call failed!
                 }
                 cout << "[" << it->getSerial() << "] " << it->getCommand() << " - Sending SIGTERM... "; //flush?
                 sleep_for(seconds(5));
                 int wait = waitpid(it->getPid(), NULL, WNOHANG);
                 if (wait < 0){
                     cerr << "smash error: > wait for child failed" << endl;
                     return 1;
                 }
                 if (wait > 0){
                     cout << "Done" <<endl;
                 }
                 if (wait == 0){
                     if (kill(it->getPid(), SIGKILL) == -1){
                         cerr << "smash error: > kill" << it->getSerial() << "– cannot send signal\n" << endl;
                         return 1; //system call failed!
                     }
                     cout << " (5 sec passed) Sending SIGKILL... Done" << endl;
                 }
             }
             exit(0);
        }
         cerr << "Illegal KILL command!" << endl;

     }
    return 1;
}




 //if (!strcmp(cmd, "quit"))


/*
 *
 */
int testKill(){
    if (!strcmp(cmd, "kill")){
        if (num_arg != 2){
            perror("Number of parameters doesn't match!\n");
            return 1;
        }
        modifyJobList();
        Job* currentJob = findJobSerial(stoi(args[2]));
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
    if (!strcmp(cmd, "jobs")){
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


//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
    int pID;
        switch(pID = fork())
    {
            case -1:
                    // Add your code here (error)
                    cerr << "smash error: > " << cmdString << endl;
            return;
                    /*
                    your code
                    */
            case 0 :
                    // Child Process
                       setpgrp();
            if (execvp(args[0], args) ==-1){
                cerr << "you used execvp from ExeExternal but failed to exec" << cmdString << endl;
                if (kill(getpid(), SIGKILL) == -1){
                    cerr << "I am a child, excecvp failed + can't commit suicide " << endl;
                    cerr << "CHECK!! sigKILL falied! " << endl;
                    return;
                }
            }
        
                    // Add your code here (execute an external command)
                    
                    /*
                    your code
                    */
            
            default:
            fg_pid=pID;
            fg_cmd=cmdString;
           if (waitpid(fg_pid, NULL, WUNTRACED) ==-1){
                //assigned NULL for status so we can run
                 cerr << "waitpid for child ExeCVP failed" << endl;
                return;
            }
            fg_pid=0;
            fg_cmd = "SMASH";
            return;
            // !!CHECK: need to print sig cont etc
                    // Add your code here
                    
                    /*
                    your code
                    */
    }
}


int BgCmd(char* lineSize, void* jobs)
{

    char* Command;
    //char* delimiters = " \t\n";
    char *args[MAX_ARG];
    if (lineSize[strlen(lineSize)-2] == '&')
    {
        lineSize[strlen(lineSize)-2] = '\0';
        // Add your code here (execute a in the background)

        char cmdString[MAX_LINE_SIZE];
        strcpy(cmdString, lineSize);
        cmdString[strlen(lineSize)-1]='\0';
        int i = 0, num_arg = 0;
        Command = strtok(lineSize, delimiters);
        if (Command == NULL) return 0;
        
        args[0] = Command;
        for (i=1; i<MAX_ARG; i++)
        {
            args[i] = strtok(NULL, delimiters);
            if (args[i] != NULL)
                num_arg++;
        }

        int pID;
            switch(pID = fork())
        {
                case -1:
                        // Add your code here (error)
                    std::cerr << "smash error: > " << cmdString << std::endl;
                    return -1;

                case 0 :
                        // Child Process
                           setpgrp();
                    //???? when execvp happens it's like a bg cmd??
                    if (execvp(args[0], args) ==-1){
                        cerr << "you used execvp from bgCMD but failed to exec" << cmdString << endl;
                        if (kill(getpid(), SIGKILL) == -1){
                            cerr << "I am a child, excecvp failed + can't commit suicide " << endl;
                            cerr << "CHECK!! sigKILL falied! " << endl;
                            return -1;
                        }
                    }
                
                default:
                    Job jobBG(cmdString, pID, false);
                    jobsVector.push_back(jobBG);
                return 0;
        }
    }

    return -1;
}

/*
* ****CHECK when to modify - might be a problem with kill
*/

void ctrl_Z_handler(int i){
    if (fg_pid <= 0)
        return;
    sigset_t mask_set;
    sigset_t old_set;
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);
    if (kill(fg_pid, SIGTSTP) == -1){
        cerr << "smash error: > kill " << fg_cmd << " – cannot send signal" << endl;
        sigprocmask(SIG_SETMASK, &old_set, &mask_set);
        return;
    }
    modifyJobList();
    currentJob=findJobPID(fg_pid);
    if (currentJob == NULL){
        Job newJob=Job(fg_cmd, fg_pid, true);
        jobsVector.push_back(newJob);
    }
    else{
        currentJob->setStopped_(true);
    }
    sigprocmask(SIG_SETMASK, &old_set, &mask_set);
    return;
}


/*
 * ****CHECK when to modify - might be a problem with kill
 */

void ctrl_C_handler(int i){
    if (fg_pid <= 0)
        return;
    sigset_t mask_set;
    sigset_t old_set;
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);
    if (kill(fg_pid, SIGINT) == -1){
        cerr << "smash error: > kill " << fg_cmd << " – cannot send signal" << endl;
        sigprocmask(SIG_SETMASK, &old_set, &mask_set);
        return;
    }
    
    modifyJobList();
    currentJob=findJobPID(fg_pid);
    if (currentJob != NULL){
        //jobsVector.erase(currentJob);
    }
    else{
        currentJob->setStopped_(true);
    }
    sigprocmask(SIG_SETMASK, &old_set, &mask_set);
    return;
}



/*
 * tests of draft
 */
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
    

    
    //Job(string command, int pid, bool stopped) : command_(command), pid_(pid), stopped_(stopped){
    Job job1("cd .. &", 123, true);
    Job job2("cd -", 127, false);
    Job job3("pwd", 131, true);
    
    jobsVector.push_back(job1);
    jobsVector.push_back(job2);
    jobsVector.push_back(job3);
    
    for (int i=0; i<4; i++){
        cout << "The job we return here is : " << i << endl;
        currentJob =findJobSerial(i);
        if (currentJob != NULL)
            currentJob->printJob();
        cout << "shem mashmauti " << endl;
    }
    
    
    for (std::vector<Job>::iterator it=jobsVector.begin(); it != jobsVector.end(); ++it)
        it->printJob();
    
    
    signal(SIGTSTP,cnt_z_handler);
    signal(SIGINT,cnt_c_handler);
    
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


/* **** STOI test
cout << "Test stoi : \n" << endl;
string num = "5";
string negative = "-1";
int myint1 = stoi(num);
cout << myint1 << endl;
int myint2 = stoi(negative);
cout << myint2 << endl;


if (myint2 < 0)
    myint2 = myint2*(-1);
 cout << myint2 << endl;
*/


/* **** TIME test
 //test of time stamp. how long a process was in jobs
 time_t realtime = time(NULL);
 std::cout << realtime << endl;   //count from 1957
 cout << time(NULL) - realtime << endl;
 */




/* **** STD test
 //using std seperately
 put initialiser in h file
 using std::cout;
 using std::endl;
 using std::string;
 */



/* **** SLEEP test
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




/* **** STRING LIST test
 string myints[] = {"a", "b", "c","d","e"};
 std::list<string> cmdHistory; // (myints,myints+5);
 */


/* **** STRING SIZE test
 char check[MAX_LINE_SIZE] ="";
 cout << strlen(check) << endl;
 strcpy(check, lineSize);
 cout << check << endl;
 cout << strlen(check) << endl;
 cout << lineSize << endl;
 cout << strlen(lineSize) << endl;
 */

/* **** MALLOC test
 int* d =(int*)malloc(sizeof(int));
 */



/* **** CHAR[] test
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



