/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: dibbler-server.cpp,v 1.12 2005-01-30 23:12:28 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2005/01/30 22:53:28  thomson
 * *** empty log message ***
 *
 * Revision 1.10  2004/12/07 00:45:42  thomson
 * Clnt managers creation unified and cleaned up.
 *
 * Revision 1.9  2004/12/02 00:51:06  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.8  2004/06/04 16:55:27  thomson
 * *** empty log message ***
 *
 * Revision 1.7  2004/06/04 14:15:53  thomson
 * Command line parsing.
 *
 *                                                                           
 */

#include <fstream>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include "DHCPServer.h"
#include "Portable.h"
#include "Logger.h"

TDHCPServer * ptr;

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
    unlink(SRVPID_FILE);
    ofstream pidfile(SRVPID_FILE);
    Log(Notice) << "My pid (" << getpid() << ") is stored in " << SRVPID_FILE << LogEnd;
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
    unlink(SRVPID_FILE);
    ofstream pidfile(SRVPID_FILE);
    cout << "My pid (" << getpid() << ") is stored in " << SRVPID_FILE << endl;
    pidfile << getpid();
    pidfile.close();

    if (chdir(WORKDIR)) {
	cout << "Unable to change directory to " << WORKDIR << logger::endl;
    }
}

void die() {
    unlink(SRVPID_FILE);
}

void signal_handler(int n) {
    Log(Crit) << "Signal received. Shutting down." << LogEnd;
    ptr->stop();
}

int getClientPID() {
    ifstream pidfile(CLNTPID_FILE);
    if (!pidfile.is_open()) 
	return -1;
    int pid;
    pidfile >> pid;
    return pid;
}

int getServerPID() {
    ifstream pidfile(SRVPID_FILE);
    if (!pidfile.is_open()) 
	return -1;
    int pid;
    pidfile >> pid;
    return pid;
}

int getRelayPID() {
    ifstream pidfile(RELPID_FILE);
    if (!pidfile.is_open()) 
	return -1;
    int pid;
    pidfile >> pid;
    return pid;
}

int status() {
    int pid = getServerPID();
    int result = pid;
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

    pid = getRelayPID();
    if (pid==-1) {
	cout << "Dibbler relay : NOT RUNNING." << endl;
    } else {
	cout << "Dibbler relay : RUNNING, pid=" << pid << endl;
    }

    return result;
}

int run() {
    TDHCPServer srv(SRVCONF_FILE);
    
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
    cout << " dibbler-server-linux ACTION" << endl
	 << " ACTION = status|start|stop|run" << endl
	 << " status    - show status and exit" << endl
	 << " start     - start installed service" << endl
	 << " stop      - stop installed service" << endl
	 << " install   - Not available in Linux/Unix systems." << endl
	 << " uninstall - Not available in Linux/Unix systems." << endl
	 << " run       - run in the console" << endl
	 << " help      - displays usage info." << endl;
}

int main(int argc, char * argv[])
{
    char command[256];
    int result=-1;
    int ret = 0;

    std::cout << DIBBLER_COPYRIGHT1 << " (SERVER)" << std::endl;
    std::cout << DIBBLER_COPYRIGHT2 << std::endl;
    std::cout << DIBBLER_COPYRIGHT3 << std::endl;
    std::cout << DIBBLER_COPYRIGHT4 << std::endl;

    logger::setLogName("Server");
    logger::Initialize(SRVLOG_FILE);

    logger::EchoOff();
    Log(Emerg) << DIBBLER_COPYRIGHT1 << " (SERVER)" << LogEnd;
    logger::EchoOn();

    // parse command line parameters
    if (argc>1) {
	strncpy(command,argv[1],strlen(argv[1])+1);    
    } else {
	memset(command,0,256);
    }

    if (!strncasecmp(command,"start",5) ) {
	result = start();
    }
    if (!strncasecmp(command,"run",3) ) {
	init();
	run();
	result = 0;
    }
    if (!strncasecmp(command,"stop",4)) {
	stop();
	result = 0;
    }
    if (!strncasecmp(command,"status",6)) {
	status();
	result = 0;
    }
    if (!strncasecmp(command,"help",4)) {
	help();
	result = 0;
    }
    if (!strncasecmp(command,"install",7)) {
	cout << "Function not available in Linux/Unix systems." << endl;
	result = 0;
    }
    if (!strncasecmp(command,"uninstall",9)) {
	cout << "Function not available in Linux/Unix systems." << endl;
	result = 0;
    }

    if (result!=0) {
	help();
    }

    logger::Terminate();

    return ret;
}
