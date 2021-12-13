#include "signals.h"
#include "job.h"
#include "commands.h"

extern int fg_pid;
extern string fg_cmd;
extern Job* currentJob;
extern vector<Job> jobsVector;
extern int Daddy;


void ctrl_Z_handler(int i){
    if (getpid() != Daddy) return;
    
    sigset_t mask_set;
    sigset_t old_set;
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);
    if (kill(fg_pid, SIGTSTP) == -1){
        //cerr << "smash error: > kill " << fg_cmd << " – cannot send signal" << endl;
        perror("smash error: > ");
        sigprocmask(SIG_SETMASK, &old_set, &mask_set);
        return;
    }
    //modifyJobList(); //unnecassary
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


void ctrl_C_handler(int i){
    if (fg_pid <= 0)
        return;
    sigset_t mask_set;
    sigset_t old_set;
    sigfillset(&mask_set);
    sigprocmask(SIG_SETMASK, &mask_set, &old_set);
    if (kill(fg_pid, SIGINT) == -1){
        //cerr << "smash error: > kill " << fg_cmd << " – cannot send signal" << endl;
        perror("smash error: > ");
        sigprocmask(SIG_SETMASK, &old_set, &mask_set);
        return;
    }
    
    modifyJobList();

    sigprocmask(SIG_SETMASK, &old_set, &mask_set);
    return;
}
