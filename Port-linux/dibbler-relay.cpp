/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: dibbler-relay.cpp,v 1.1 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *                                                                           
 */

#include <fstream>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include "DHCPRelay.h"
#include "Portable.h"
#include "Logger.h"

TDHCPRelay * ptr;

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
	    cout << "Can't fork first child." << endl;
	    return;
	} else if (childpid > 0) 
	    exit(0); // parent process
	
	if (setpgrp() == -1) {
	    cout << "Can't change process group." << endl;
	    return;
	}
	
	signal( SIGHUP, SIG_IGN);
	
	if ( (childpid = fork()) <0) {
	    cout << "Can't fork second child." << endl;
	    return;
	} else if (childpid > 0)
	    exit(0); // first child
	
    } // getppid()!=1

    // store new pid file
    string tmp;
    tmp = (string)WORKDIR+"/"+(string)RELPID_FILE;
    unlink(tmp.c_str());
    ofstream pidfile(tmp.c_str());
    Log(Notice) << "My pid (" << getpid() << ") is stored in " << tmp << LogEnd;
    pidfile << getpid();
    pidfile.close();
    umask(0);
}

void daemon_die() {
    logger::Terminate();
    logger::EchoOn();
}

void init() {
    string tmp;
    tmp = (string)WORKDIR+"/"+(string)RELPID_FILE;
    unlink(tmp.c_str());
    ofstream pidfile(tmp.c_str());
    cout << "My pid (" << getpid() << ") is stored in " 
	 << tmp << endl;
    pidfile << getpid();
    pidfile.close();

    if (chdir(WORKDIR)) {
	cout << "Unable to change directory to " << WORKDIR << logger::endl;
    }
}

void die() {
    string tmp = (string)WORKDIR+"/"+(string)RELPID_FILE;
    unlink(tmp.c_str());
}

void signal_handler(int n) {
    Log(Crit) << "Signal received. Shutting down." << LogEnd;
    ptr->stop();
}

int getClientPID() {
    string tmp = (string)WORKDIR+"/"+(string)CLNTPID_FILE;
    ifstream pidfile(tmp.c_str());
    if (!pidfile.is_open()) 
	return -1;
    int pid;
    pidfile >> pid;
    return pid;
}

int getServerPID() {
    string tmp = (string)WORKDIR+"/"+(string)SRVPID_FILE;
    ifstream pidfile(tmp.c_str());
    if (!pidfile.is_open()) 
	return -1;
    int pid;
    pidfile >> pid;
    return pid;
}

int status() {
    int pid = getServerPID();
    if (pid==-1) {
	cout << "Dibbler server: NOT RUNNING." << endl;
    } else {
	cout << "Dibbler server: RUNNING, pid=" << pid << endl;
    }
    
    pid = getClientPID();
    if (pid==-1) {
	cout << "Dibbler client: NOT RUNNING." << endl;
    } else {
	cout << "Dibbler client: RUNNING, pid=" << pid << endl;
    }

    return 0;
}

int run() {
    TDHCPRelay srv(RELCONF_FILE);
    
    ptr = &srv;
    
    // connect signals
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    srv.run();
    
    die();
    return 0;
}

int start() {
    init();
    daemon_init();

    run();

    daemon_die();
    return 0;
}

int stop() {
    int pid = getServerPID();
    if (pid==-1) {
	cout << "Server is not running." << endl;
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

void help() {
    cout << "Usage:" << endl;
    cout << " dibbler-relay ACTION" << endl
	 << " ACTION = status|start|stop|install|uninstall|run" << endl
	 << " status    - show status and exit" << endl
	 << " start     - start installed service" << endl
	 << " stop      - stop installed service" << endl
	 << " install   - install service [NOT IMPLEMENTED YET]" << endl
	 << " uninstall - uninstall service [NOT IMPLEMENTED YET]" << endl
	 << " run       - run in the console" << endl
	 << " help      - displays usage info." << endl;
}

int main(int argc, char * argv[])
{
    char command[256];
    int result=-1;

    std::cout << DIBBLER_COPYRIGHT1 << " (RELAY)" << std::endl;
    std::cout << DIBBLER_COPYRIGHT2 << std::endl;
    std::cout << DIBBLER_COPYRIGHT3 << std::endl;
    std::cout << DIBBLER_COPYRIGHT4 << std::endl;

    logger::setLogName("Relay");
    logger::Initialize(WORKDIR"/"SRVLOG_FILE);

    logger::EchoOff();
    Log(Emerg) << DIBBLER_COPYRIGHT1 << " (RELAY)" << LogEnd;
    logger::EchoOn();

    // parse command line parameters
    if (argc>1) {
	strncpy(command,argv[1],strlen(argv[1]));    } else {
	memset(command,0,256);
    }

    if (!strncasecmp(command,"start",5) ) {
	result = start();
    }
    if (!strncasecmp(command,"run",3) ) {
	init();
	result = run();
    }
    if (!strncasecmp(command,"stop",4)) {
	result = stop();
    }
    if (!strncasecmp(command,"status",6)) {
	result = status();
    }
    if (!strncasecmp(command,"help",4)) {
	help();
	result = 0;
    }
    if (!strncasecmp(command,"install",7)) {
	cout << "NOT IMPLEMENTED YET" << endl;
	result = 0;
    }
    if (!strncasecmp(command,"uninstall",9)) {
	cout << "NOT IMPLEMENTED YET" << endl;
	result = 0;
    }

    if (result!=0) {
	help();
    }

    logger::Terminate();

    return 0;
}
