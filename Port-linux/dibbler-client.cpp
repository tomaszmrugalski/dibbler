/*  Dibbler: a portable DHCPv6
    Copyright (C) 2003 Tomasz Mrugalski <thomson@klub.com.pl>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details. */

#include <fstream>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include "DHCPClient.h"
#include "Portable.h"
#include "Logger.h"

TDHCPClient * ptr;

void signal_handler(int n) {
    Log(Crit) << "Signal received. Shutting down." << LogEnd;
    ptr->stop();
}

void daemon_init() {

    //FIXME: daemon should close all open files
    //fclose(stdin);
    //fclose(stdout);
    //fclose(stderr);

    int childpid;
    Log(Notice) << "Starting daemon..." << LogEnd;

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
	    Log(Error) << "Can't fork first child." << LogEnd;
	    return;
	} else if (childpid > 0) 
	    exit(0); // parent process
	
	if (setpgrp() == -1) {
	    Log(Error) << "Can't change process group." << LogEnd;
	    return;
	}
	
	signal( SIGHUP, SIG_IGN);
	
	if ( (childpid = fork()) <0) {
	    Log(Error) << "Can't fork second child." << LogEnd;
	    return;
	} else if (childpid > 0)
	    exit(0); // pierwszy potomek
	
    } // getppid()!=1

    unlink(CLNTPID_FILE);
    ofstream pidfile(CLNTPID_FILE);
    pidfile << getpid();
    pidfile.close();
    Log(Notice) << "My pid (" << getpid() << ") is stored in " << CLNTPID_FILE << LogEnd;

    umask(0);
}

void daemon_die() {
    logger::EchoOn();
}

void init() {
    unlink(CLNTPID_FILE);
    ofstream pidfile(CLNTPID_FILE);
    Log(Notice) << "My pid (" << getpid() << ") is stored in " << CLNTPID_FILE << LogEnd;
    pidfile << getpid();
    pidfile.close();

    if (chdir(WORKDIR)) {
	Log(Error) << "Can't change directory to " << WORKDIR << LogEnd;
    }
}

void die() {
    Log(Debug) << "Removing PID file (" << CLNTPID_FILE << ")." << LogEnd;
    unlink(CLNTPID_FILE);
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
    int result;
    if (pid==-1) {
	cout << "Dibbler server: NOT RUNNING." << endl;
    } else {
	cout << "Dibbler server: RUNNING, pid=" << pid << endl;
    }
    
    pid = getClientPID();
    result = pid;
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

    return pid;
}

int run() {
    TDHCPClient client(CLNTCONF_FILE);
    ptr = &client;

    // connect signals
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    client.run();

    die();
    return 0;
}

int start() {
    daemon_init();
    run();
    daemon_die();
    return 0;
}

int stop() {
    int pid = getClientPID();
    if (pid==-1) {
	Log(Crit) << "Client is not running." << endl;
	return -1;
    }
    Log(Crit) << "Sending KILL signal to process " << pid << endl;
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
    cout << " dibbler-client-linux ACTION" << endl
	 << " ACTION = status|start|stop|install|uninstall|run" << endl
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
    int ret=0;

    std::cout << DIBBLER_COPYRIGHT1 << " (CLIENT)" << std::endl;
    std::cout << DIBBLER_COPYRIGHT2 << std::endl;
    std::cout << DIBBLER_COPYRIGHT3 << std::endl;
    std::cout << DIBBLER_COPYRIGHT4 << std::endl;

    logger::setLogName("Client");
    logger::Initialize(CLNTLOG_FILE);

    logger::EchoOff();
    Log(Notice) << DIBBLER_COPYRIGHT1 << " (CLIENT)" << LogEnd;
    logger::EchoOn();

    // parse command line parameters
    if (argc>1) {
	strncpy(command,argv[1],strlen(argv[1])+1);
    } else {
	memset(command,0,256);
    }

    if (!strncasecmp(command,"start",5) ) {
	start();
	result = 0;
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

