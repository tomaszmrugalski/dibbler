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

    logger::Initialize(CLNTLOG_FILE);

    int fd,childpid;
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
    
    chdir("/");

    umask(0);
}

void daemon_die() {
    logger::Terminate();
}

void init() {
    unlink(RESOLVCONF_FILE);

    string tmp = (string)WORKDIR+"/"+(string)CLNTPID_FILE;
    unlink(tmp.c_str());
    ofstream pidfile(tmp.c_str());
    std::clog << logger::logNotice << "My pid (" << getpid() << ") is stored in " 
	      << tmp << logger::endl;
    pidfile << getpid();
    pidfile.close();
}

void die() {
    string tmp = (string)WORKDIR+"/"+(string)CLNTPID_FILE;
    unlink(tmp.c_str());
}

int main(int argc, char * argv[])
{
    bool daemon_mode = false;

    logger::setLogname("Client");

    // parse command line parameters
    // Well, one big FIXME here :)
    if (argc==2 && !strncasecmp("-d",argv[1],2) ) {
	daemon_mode = true;
	daemon_init();
    }
    
    init();

    TDHCPClient client(CLNTCONF_FILE);
    ptr = &client;

    // connect signals
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    client.run();

    die();

    if (daemon_mode)
	daemon_die();

    return 0;
}

