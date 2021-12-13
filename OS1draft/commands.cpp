#include "commands.h"
#include "job.h"
#include "includes.h"

char* cmd;
char* args[MAX_ARG];

extern int fg_pid;
extern string fg_cmd;
extern vector<Job> jobsVector;
extern Job* currentJob;


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

void modifyJobList(){
    pid_t wait;
    int status;

    for (std::vector<Job>::iterator it=jobsVector.begin(); it != jobsVector.end(); ++it){
        if (it->isStopped_() == true)
            continue;
        wait = waitpid(it->getPid(), &status, WNOHANG | WUNTRACED); //  //it->getPid()
        if (wait == -1){
            if (errno == ECHILD){
                jobsVector.erase(it);
                --it;
            }
            perror("smash error: > "); // ? ? ?
        }
        if (wait > 0){
            if ( WIFEXITED(status) || WIFSIGNALED(status) ){
                jobsVector.erase(it);
                --it;
            }
        }
    }
}

Job* findJobSerial(int serial){
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


//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(char* lineSize, char* cmdString){
	char* cmd; 
	char* args[MAX_ARG];
	//char pwd[MAX_LINE_SIZE];
	//char* delimiters = " \t\n";
	int i = 0, num_arg = 0;
	bool illegal_cmd = false; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
 
	}
 
    
    
    char tmpy[MAX_LINE_SIZE]="";
    strcpy(tmpy, cmdString); //strcpy(char[] , char*)
    if (strcmp(tmpy, "history")){
        if (cmdHistory.size() == MAXHISTORY ){
            cmdHistory.pop_front();
        }
        cmdHistory.push_back(string(tmpy));
    }


    
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ){
        if (num_arg == 0){
            //cerr << "Not enough parameters!" << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }
        else if(num_arg > 1){
            //cerr << "Too many parameters!" << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }
        if(!strcmp(args[1], dash)){  //enter if '-'
            if (strlen(oldPwd) == 0){
                cerr << "smash error: > There is no older path for cd - " << endl;
                //printf("smash error: > \"%s\"\n", cmdString); //?
                return 1;
            }
            else{
                strcpy(nextPwd, oldPwd); //oldPwd isn't NULL
            }
        } //finish if '-'
        else{
            strcpy(nextPwd, args[1]);
        }
        char* buffCD=NULL;
        buffCD =getcwd(buffCD, 0);
        if (buffCD == NULL){
            perror("smash error: > ");
            return 1;
        }
            
        strcpy(tmpPwd, buffCD); //tmpPwd <- current directory
        if(chdir(nextPwd) == -1){
           // cerr << "smash error: > \"" << nextPwd << "\" - No such file or directory" << endl;
            perror("smash error: > ");
            free(buffCD);
            return 1;
        }
        free(buffCD);
        char* buffAgain=NULL;
        buffAgain = getcwd(buffAgain, 0);
        if (buffAgain == NULL){
            perror("smash error: > ");
            return 1;
        }
        
        if (!strcmp(args[1], dash))
            cout << buffAgain << endl;
        strcpy(oldPwd, tmpPwd); //enter last cwd into oldPwd
        free(buffAgain);
        return 0;
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")){
        if (num_arg != 0){
            //cerr << "Too many parameters!" << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }

        char* buffCWD=NULL;
        buffCWD = getcwd(buffCWD, 0);
        if (buffCWD == NULL){
            //cerr << "falied to retreive current working directory" << endl;
            perror("smash error: > ");
            return 1;
        }
        cout << buffCWD << endl;
        free(buffCWD);
        return 0;
	}
	
	/*************************************************/
	//else if (!strcmp(cmd, "mkdir")){}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")){
        if (num_arg != 0){
            //cerr << "Too many parameters!" << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }
        modifyJobList();
        for (std::vector<Job>::iterator it=jobsVector.begin(); it != jobsVector.end(); ++it){
            it->printJob();
        }
    }
    
    
	/*************************************************/
	else if (!strcmp(cmd, "showpid")){
        if(num_arg > 0){
            //cerr << "Too many parameters!" << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
         }
         cout << "smash pid is " << getpid() << endl;
         return 0;
	}
    
	/*************************************************/
	else if (!strcmp(cmd, "fg")){
        Job* currentJob = NULL;
        if (num_arg > 1){
            //cerr << "Too many parameters!" << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }
        modifyJobList();
        if (num_arg == 1){
            
            int stringToInt;
            try {
                stringToInt =  stoi(args[1]);
            }
            catch (...) {
                printf("smash error: > \"%s\"\n", cmdString);
                return 1;
            }
            
            
            currentJob = findJobSerial(stringToInt);
            if (currentJob == NULL){
                cerr << "smash error: > " << args[1] << " - job does not exist" << endl;
                return 1;
            }
        }
        
        if (num_arg == 0){
            if ( jobsVector.size() == 0 ){
                cerr << "smash error: > " << " job list is empty" << endl;
                return 1;
            }
            else{
                std::vector<Job>::reverse_iterator rit=jobsVector.rbegin();
                currentJob = &(*rit);
            }
        }
        currentJob->printJob();
        //update the command to run in FG.
        //use the command pid to actually run it over smash
        
        if(currentJob->isStopped_() == true){
            if (kill(currentJob->getPid(), SIGCONT) == -1){
                //cerr << "smash error: > kill " << currentJob->getSerial() << " – cannot send signal" << endl;
                perror("smash error: > ");
                return 1;
            }
        }
        currentJob->setStopped_(false);
        fg_pid=currentJob->getPid();
        fg_cmd = currentJob->getCommand();  //consider char* or strcpy etc
        cout << currentJob->getCommand() << endl;
        if (waitpid(fg_pid, NULL, WUNTRACED) ==-1){
            //assigned NULL for status so we can run
            //cerr << " @@@@@@@ waitpid for FG failed at fgcmd with: " << fg_pid << endl;
            perror("smash error: > ");
            return 1;
        }
        modifyJobList();
        fg_pid=0;
        fg_cmd = "smash";
        return 0;
	}
    
	/*************************************************/
	else if (!strcmp(cmd, "bg")){
        Job* currentJob = NULL;
        if (num_arg > 1){
            //cerr << "Too many parameters!" << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }
        modifyJobList();
        if (num_arg == 1){
            
            int stringToInt;
            try {
                stringToInt =  stoi(args[1]);
            }
            catch (...) {
                printf("smash error: > \"%s\"\n", cmdString);
                return 1;
            }
            
            currentJob = findJobSerial(stringToInt);
            if (currentJob == NULL){
                cerr << "smash error: > " << args[1] << " - job does not exist" << endl;
                return 1;
            }
            if (currentJob->isStopped_() == false){
                cerr << "smash error: > " << args[1] << " job already runs in background" << endl;
                //printf("smash error: > \"%s\"\n", cmdString);
                return 1;
            }
            // send kill to check if stopped
            //cout << args[1] << endl;
            // return 0;
        }
        
        if (num_arg == 0){
            for (std::vector<Job>::reverse_iterator rit=jobsVector.rbegin(); rit != jobsVector.rend(); ++rit){
                if (rit->isStopped_() == true){
                    currentJob = &(*rit); //
                    break; //MUST exit this for loop
                }
            }
            if (currentJob==NULL){
                cerr << "smash error: > " << " no stopped jobs in the background" << endl;
                //printf("smash error: > \"%s\"\n", cmdString);
                return 1;
            }
        }
        
        if (kill(currentJob->getPid(), SIGCONT) == -1){
            //cerr << "smash error: > kill " << currentJob->getSerial() << " – cannot send signal" << endl;
            perror("smash error: > ");
            return 1;
        }
        else{
            currentJob->setStopped_(false);
            cout << currentJob->getCommand() << endl;
        }
        return 0;
	}
    
	/*************************************************/
	else if (!strcmp(cmd, "quit")){
        if (num_arg > 1){
            //cerr << "smash error: > too many argument for quit" << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }
        if (num_arg == 0)
            exit(0);

        modifyJobList();
        if (num_arg ==1 && (!strcmp(args[1], "kill") ) ){
            
             for (std::vector<Job>::iterator it=jobsVector.begin(); it != jobsVector.end(); ++it){
                 // ? ? ?
                 if (kill(it->getPid(), SIGTERM) == -1){ // ? ? ?
                     if (errno == ESRCH){
                         continue;
                     }
                     //cerr << "smash error: > kill " << it->getSerial() << " – cannot send signal" << endl;
                     perror("smash error: > "); //system call failed!
                     //return 1;
                     exit(1);
                 }
                 cout << "[" << it->getSerial() << "] " << it->getCommand() << " - Sending SIGTERM... "; //flush?
                 int wait=0;
                 int i=0;
                 for (i=0; i<5; i++){
                     
                     wait = waitpid(it->getPid(), NULL, WNOHANG);
                     if (wait < 0){
                         //cerr << "smash error: > wait for child failed" << endl;
                         perror("smash error: > ");
                         //return 1; //? perhaps exit(1) ?
                         exit(1);
                     }
                     if (wait > 0){
                         cout << "Done." <<endl;
                         break;
                     }
                     sleep_for(seconds(1));
                 }
                 //cout << "I slept for " << i << "seconds! " << endl;
                 if (wait == 0){
                     
                     if (kill(it->getPid(), SIGKILL) == -1){
                         //cerr << "smash error: > kill " << it->getSerial() << " – cannot send signal" << endl;
                         perror("smash error: > ");
                         //return 1; //system call failed!
                         exit(1);
                     }
                     cout << "(5 sec passed) Sending SIGKILL... Done." << endl;
                 }

             }
            modifyJobList();
             exit(0);
        }
        //cerr << "Illegal KILL command!" << endl;
        printf("smash error: > \"%s\"\n", cmdString);
        return 1;
	} 
	/*************************************************/
    
    else if (!strcmp(cmd, "history")){
        if (num_arg != 0){
            //cerr << "Too many parameters! << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }
        for (std::list<string>::iterator it=cmdHistory.begin(); it != cmdHistory.end(); ++it)
          std::cout << *it << endl;
        return 0;
    }
    
    else if (!strcmp(cmd, "kill")){
        if (num_arg != 2){
            //cerr << "Number of parameters doesn't match!" << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }
        modifyJobList();
        
        int stringToInt;
        try {
            stringToInt =  stoi(args[2]);
        }
        catch (...) {
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }
        
        Job* currentJob = findJobSerial(stringToInt);
        if (currentJob == NULL){
            cerr << "smash error: > kill " << args[2] << " – job does not exist" << endl;
            return 1;
        }

                
        int killNum;
        try {
                   killNum =  stoi(args[1]);
               }
               catch (...) {
                   printf("smash error: > \"%s\"\n", cmdString);
                   return 1;
               }
        
        if (killNum < 0)
            killNum = killNum*(-1);
        else {
            //cerr << "illegal signal parameter!" << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }
        if ( killNum>31){
            //cerr << "illegal signal parameter! bigger than 31" << endl;
            printf("smash error: > \"%s\"\n", cmdString);
            return 1;
        }
        if (kill(currentJob->getPid(), killNum) == -1){
            //cerr << "smash error: > kill " << whichSignal(killNum) << " – cannot send signal" << endl;
            cerr << "smash error: > kill " << args[2] << " – cannot send signal" << endl;
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
        return 0;
    }
    
    
    else if (!strcmp(cmd, "diff")){
        int file1, file2;
        ssize_t bytesRead1, bytesRead2;
        const int nbytes=1;
        
        file1 = open(args[1], O_RDONLY);
        if (file1 == -1){
            perror("smash error: > ");
            //cerr << "FILE1 ERROR: No such file or directory" << endl;
            return 1;
        }
        file2 = open(args[2], O_RDONLY);
        if (file2 == -1){
            perror("smash error: > ");
            //cerr << "FILE2 ERROR: No such file or directory" << endl;
            if (close(file1) == -1){
                perror("smash error: > ");
                //cerr << "FILE1 ERROR: close file failed!" << endl;
            }
            return 1;
        }
        
        char buff1 = 'a';
        char buff2 = 'a';
        
        bytesRead1 = read(file1, &buff1, nbytes);
        if(bytesRead1==-1){
            perror("smash error: > ");
            if (close(file1) == -1 )
                perror("smash error: > ");
            if (close(file2) == -1 )
                perror("smash error: > ");
            return 1;
        }
        bytesRead2 = read(file2, &buff2, nbytes);
        if(bytesRead2==-1){
            perror("smash error: > ");
            if (close(file1) == -1 )
                perror("smash error: > ");
            if (close(file2) == -1 )
                perror("smash error: > ");
            return 1;
        }
        bool isLegal = true;
        while(bytesRead1!=0 || bytesRead2!=0){
            
            if (bytesRead1==0 || bytesRead2==0){
                cout << "1" <<endl; //i.e. files are differnet
                if (close(file1) == -1 ){
                    perror("smash error: > ");
                    isLegal=false;
                }
                if (close(file2) == -1 ){
                    perror("smash error: > ");
                    isLegal=false;
                }
                if (isLegal == false)
                    return 1;
                else
                    return 0;
            }
        
            if (buff1!=buff2){// files were equal so far
                cout << "1" <<endl; //i.e. files are differnet
                if (close(file1) == -1 ){
                    perror("smash error: > ");
                    isLegal=false;
                }
                if (close(file2) == -1 ){
                    perror("smash error: > ");
                    isLegal=false;
                }
                if (isLegal == false)
                    return 1;
                else
                    return 0;
            }
            
            bytesRead1 = read(file1, &buff1, nbytes);
            if(bytesRead1==-1){
                perror("smash error: > ");
                if (close(file1) == -1 )
                    perror("smash error: > ");
                if (close(file2) == -1 )
                    perror("smash error: > ");
                return 1;
            }
            bytesRead2 = read(file2, &buff2, nbytes);
            if(bytesRead2==-1){
                perror("smash error: > ");
                if (close(file1) == -1 )
                    perror("smash error: > ");
                if (close(file2) == -1 )
                    perror("smash error: > ");
                return 1;
            }
        }
        cout << "0" <<endl; //i.e. files are identical
        
        if (close(file1) == -1 ){
            perror("smash error: > ");
            isLegal=false;
        }
        if (close(file2) == -1 ){
            perror("smash error: > ");
            isLegal=false;
        }
        if (isLegal == false)
            return 1;
        return 0;
    }
    
	else{ // external command
 		ExeExternal(args, cmdString);
	 	return 0;
	}
	if (illegal_cmd == true){
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}

//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString){
	int pID;
    	switch(pID = fork()){
                case -1:
                perror("smash error: > "); //fork failed
                return;
                
                case 0 : //Child process enters here
                setpgrp(); // notice that wait is going to fail while using setpgrp
                //cout << "I am SON with pid: " << pID << " and my fg is: " << fg_pid << endl;
                if (execvp(args[0], args) ==-1){
                    //cerr << "you used execvp from ExeExternal but failed to exec" << cmdString << endl;
                    perror("smash error: > ");
                    if (kill(getpid(), SIGKILL) == -1){
                        //cerr << "I am a child, excecvp failed + can't commit suicide " << endl;
                        //cerr << "smash error: > kill SIGKILL" << " – cannot send signal" << endl; //SIGKILL = 9
                        perror("smash error: > ");
                    return;
                    }
                }
                return; // will never be executed
                
                default:
                fg_pid=pID;
                fg_cmd=cmdString;
                int status;
                waitpid(pID, &status, WUNTRACED);
                // this waitpid will wait for son to TERMINATE or STOP but will always return -1 as setpgrp is different
                fg_pid=0;
                fg_cmd = "smash";
                return;
        }
}


//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize){
	//char ExtCmd[MAX_LINE_SIZE+2];
	//char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&"))){}
	return -1;
}


//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize){
    char* Command;
    char *args[MAX_ARG];
    char cmdString[MAX_LINE_SIZE];
    strcpy(cmdString, lineSize);
    long size = strlen(lineSize) -2;
    long loc;
    for (loc=size; loc>0; loc--){
        if ((lineSize[loc] != '&') && (lineSize[loc] !=' ' ) )
            return -1;
        else if (lineSize[loc] == '&'){
            lineSize[loc] = '\0';
            //cout << "I am LOCA! " << loc<< endl;
            break;
        }
    }
    //cmdString[strlen(lineSize)-1]='\0';
    int i = 0, num_arg = 0;
    
    Command = strtok(lineSize, delimiters);
    if (Command == NULL){
        printf("smash error: > \"%s\"\n", cmdString);
        return 0;
    }

    
    args[0] = Command;
    for (i=1; i<MAX_ARG; i++){
        args[i] = strtok(NULL, delimiters);
        if (args[i] != NULL)
            num_arg++;
    }
    
    int pID;
        switch(pID = fork()){
            case -1:
                perror("smash error: > ");
                return -1;

            case 0 : // Child Process
                setpgrp();
                if (execvp(args[0], args) ==-1){
                    //cerr << "you used execvp from bgCMD but failed to exec" << cmdString << endl;
                    perror("smash error: > ");
                    if (kill(getpid(), SIGKILL) == -1){
                        //cerr << "I am a child, excecvp failed + can't commit suicide " << endl;
                        perror("smash error: > ");
                        return -1;
                    }
                }
            
            default:
                //cout << "I am LOCA! " << loc<< endl;
                cmdString[loc] = '\0';
                Job jobBG(cmdString, pID, false);
                jobsVector.push_back(jobBG);
                
                char tmpy[MAX_LINE_SIZE]="";
                strcpy(tmpy, cmdString); //strcpy(char[] , char*)
                if (strcmp(tmpy, "history")){
                    if (cmdHistory.size() == MAXHISTORY ){
                        cmdHistory.pop_front();
                    }
                    cmdHistory.push_back(string(tmpy));
                }
                return 0;
    }
    
    return -1;
}

