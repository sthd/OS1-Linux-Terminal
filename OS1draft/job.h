#ifndef _JOB_H
#define _JOB_H
#include "includes.h"
//using namespace std;


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

#endif

