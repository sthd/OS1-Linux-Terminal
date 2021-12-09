#ifndef _INCLUDES_H
#define _INCLUDES_H

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


#endif
/*
 char victim[MAX_LINE_SIZE]="waterxnsdia";
 strcpy(victim, cmdString);
 cout << victim << endl;
 cmdHistory.push_back(victim);
 std::list<string>::iterator ity=cmdHistory.end();
 std::cout << *ity << endl;

 */


/*
    cout << "I slept for ";
    sleep_for(seconds(1));
    cout << " 1";
    sleep_for(seconds());
    cout << " 2";
    sleep_for(seconds(1));
    cout << " 3";
    sleep_for(seconds(1));
    cout << " 4";
    sleep_for(seconds(1));
    cout << "   5 seconds " << endl;
 */

/*
    if args[1] == kill we return 1
    go through jobs
    send sigterm to each job
    check if job terminated
 */

