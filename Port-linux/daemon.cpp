/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: daemon.cpp,v 1.2 2005-02-03 22:42:25 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/02/03 22:06:40  thomson
 * Linux startup/pid checking changed.
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include "Portable.h"
#include "Logger.h"

extern int status();
extern int run();

using namespace std;

int getPID(char * file) {
    ifstream pidfile(file);
    if (!pidfile.is_open()) 
	return -1;
    int pid;
    pidfile >> pid;
    return pid;
}

int getClientPID() {
    return getPID(CLNTPID_FILE);
}

int getServerPID() {
    return getPID(SRVPID_FILE);
}

int getRelayPID() {
    return getPID(RELPID_FILE);
}

void daemon_init() {

    //FIXME: daemon should close all open files
    //fclose(stdin);
    //fclose(stdout);
    //fclose(stderr);

    int childpid;
    cout << "Starting daemon..." << endl;
    logger::EchoOff();

    if (getppid()!=1) {

#ifdef SIGTTOU
	signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
	signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
	signal(SIGTSTP, SIG_IGN);
#endif
	if ( (childpid = fork()) <0 ) {
	    Log(Crit) << "Can't fork first child." << endl;
	    return;
	} else if (childpid > 0) 
	    exit(0); // parent process
	
	if (setpgrp() == -1) {
	    Log(Crit) << "Can't change process group." << endl;
	    return;
	}
	
	signal( SIGHUP, SIG_IGN);
	
	if ( (childpid = fork()) <0) {
	    cout << "Can't fork second child." << endl;
	    return;
	} else if (childpid > 0)
	    exit(0); // first child
	
    } // getppid()!=1

    umask(0);
}

void daemon_die() {
    logger::Terminate();
    logger::EchoOn();
}

int init(char * pidfile, char * workdir) {
    string tmp;
    int pid = getPID(pidfile);
    if (pid != -1) {
	Log(Crit) << "Process already running (pid=" << pid << ", file " << pidfile 
		  << " is present)." << LogEnd;
	return 0;
    }

    unlink(pidfile);
    ofstream pidFile(pidfile);
    if (!pidFile.is_open()) {
	Log(Crit) << "Unable to create " << pidfile << " file." << LogEnd;
	return 0;
    }
    pidFile << getpid();
    pidFile.close();
    Log(Notice) << "My pid (" << getpid() << ") is stored in " << pidfile << endl;

    if (chdir(workdir)) {
	Log(Crit) << "Unable to change directory to " << workdir << "." << LogEnd;
	return 0;
    }
    return 1;
}

void die(char * pidfile) {
    unlink(pidfile);
}



int start(char * pidfile, char * workdir) {
    int result;
    daemon_init();
    result = run();
    daemon_die();
    return result;
}

int stop(char * pidfile) {
    int pid = getPID(pidfile);
    if (pid==-1) {
	cout << "Process is not running." << endl;
	return -1;
    }
    cout << "Sending KILL signal to process " << pid << endl;
    kill(pid, SIGTERM);
    return 0;
}

int install() {
    return 0;
}

int uninstall() {
    return 0;
}

/** things to do just after started */
void logStart(char * note, char * logname, char * logfile) {
    std::cout << DIBBLER_COPYRIGHT1 << note << std::endl;
    std::cout << DIBBLER_COPYRIGHT2 << std::endl;
    std::cout << DIBBLER_COPYRIGHT3 << std::endl;
    std::cout << DIBBLER_COPYRIGHT4 << std::endl;

    logger::setLogName(logname);
    logger::Initialize(logfile);

    logger::EchoOff();
    Log(Emerg) << DIBBLER_COPYRIGHT1 << " " << note << LogEnd;
    logger::EchoOn();
}

/** things to do just before end 
 */
void logEnd() {
    logger::Terminate();
}
