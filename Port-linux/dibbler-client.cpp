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
    std::clog << logger::logEmerg << "Signal received. Shutting down." << logger::endl;
    ptr->stop();
}

void daemon_init() {

    //FIXME: daemon should close all open files
    //fclose(stdin);
    //fclose(stdout);
    //fclose(stderr);

    std::clog << "Switching to background..." << std::endl;

    logger::Initialize(CLNTLOG_FILE);

    int childpid;
    std::clog << logger::logNotice << "Starting daemon..." << logger::endl;

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
	    std::clog << logger::logError << "Can't fork first child." << logger::endl;
	    return;
	} else if (childpid > 0) 
	    exit(0); // parent process
	
	if (setpgrp() == -1) {
	    std::clog << logger::logError << "Can't change process group." << logger::endl;
	    return;
	}
	
	signal( SIGHUP, SIG_IGN);
	
	if ( (childpid = fork()) <0) {
	    std::clog << logger::logError << "Can't fork second child." << logger::endl;
	    return;
	} else if (childpid > 0)
	    exit(0); // pierwszy potomek
	
    } // getppid()!=1

    string tmp = (string)WORKDIR+"/"+(string)CLNTPID_FILE;
    unlink(tmp.c_str());
    ofstream pidfile(tmp.c_str());
    std::clog << logger::logNotice << "My pid (" << getpid() << ") is stored in " 
	      << tmp << logger::endl;
    pidfile << getpid();
    pidfile.close();

    umask(0);
}

void daemon_die() {
    logger::Terminate();
}

void init() {
    unlink(WORKDIR"/tmp-dns");
    unlink(WORKDIR"/tmp-domain");

    string tmp = (string)WORKDIR+"/"+(string)CLNTPID_FILE;
    unlink(tmp.c_str());
    ofstream pidfile(tmp.c_str());
    std::clog << logger::logNotice << "My pid (" << getpid() << ") is stored in " 
	      << tmp << logger::endl;
    pidfile << getpid();
    pidfile.close();

    if (chdir(WORKDIR)) {
	std::clog << logger::logError << "Can't change directory to " << WORKDIR << logger::endl;
    }

}

void die() {
    string tmp = (string)WORKDIR+"/"+(string)CLNTPID_FILE;
    unlink(tmp.c_str());
    dns_del_all();
    domain_del_all();
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
	     << " install   - install service [NOT IMPLEMENTED YET]" << endl
	     << " uninstall - uninstall service [NOT IMPLEMENTED YET]" << endl
	     << " run       - run in the console" << endl
	     << " help      - displays usage info." << endl;
}

int main(int argc, char * argv[])
{
    char command[256];
    int result=-1;

    std::cout << DIBBLER_COPYRIGHT1 << " (CLIENT)" << std::endl;
    std::cout << DIBBLER_COPYRIGHT2 << std::endl;
    std::cout << DIBBLER_COPYRIGHT3 << std::endl;
    std::cout << DIBBLER_COPYRIGHT4 << std::endl;

    logger::setLogName("Client");

    // parse command line parameters
    if (argc>1) {
	strncpy(command,argv[1],strlen(argv[1]));
    } else {
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

    return 0;

}

