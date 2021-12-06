// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"

extern int fg_pid;

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
    modifyJobList(); //unnecassary
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
    /*
     currentJob=findJobPID(fg_pid);
     if (currentJob != NULL){
         //jobsVector.erase(currentJob);
     }
     else{
         currentJob->setStopped_(true);
     }
     */

    sigprocmask(SIG_SETMASK, &old_set, &mask_set);
    return;
}
