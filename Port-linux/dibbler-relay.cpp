/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: dibbler-relay.cpp,v 1.6 2005-02-03 22:42:25 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/02/03 22:06:40  thomson
 * Linux startup/pid checking changed.
 *
 * Revision 1.4  2005/02/03 20:09:11  thomson
 * *** empty log message ***
 *
 * Revision 1.3  2005/01/30 23:12:28  thomson
 * *** empty log message ***
 *
 * Revision 1.2  2005/01/30 22:53:28  thomson
 * *** empty log message ***
 *
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 */

#include <signal.h>
#include "DHCPRelay.h"
#include "Portable.h"
#include "Logger.h"
#include "daemon.h"

TDHCPRelay * ptr;

void signal_handler(int n) {
    Log(Crit) << "Signal received. Shutting down." << LogEnd;
    ptr->stop();
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

    pid = getRelayPID();
    if (pid==-1) {
	cout << "Dibbler relay : NOT RUNNING." << endl;
    } else {
	cout << "Dibbler relay : RUNNING, pid=" << pid << endl;
    }

    return pid;
}

int run() {
    if (!init(RELPID_FILE, WORKDIR)) {
	return -1;
    }

    TDHCPRelay srv(RELCONF_FILE);
    
    ptr = &srv;
    
    // connect signals
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    srv.run();
    
    die(RELPID_FILE);
    return 0;
}

int help() {
    cout << "Usage:" << endl;
    cout << " dibbler-relay ACTION" << endl
	 << " ACTION = status|start|stop|install|uninstall|run" << endl
	 << " status    - show status and exit" << endl
	 << " start     - start installed service" << endl
	 << " stop      - stop installed service" << endl
	 << " install   - Not available in Linux/Unix systems." << endl
	 << " uninstall - Not available in Linux/Unix systems." << endl
	 << " run       - run in the console" << endl
	 << " help      - displays usage info." << endl;
    return 0;
}

int main(int argc, char * argv[])
{
    char command[256];
    int result=-1;

    logStart("(RELAY)", "Relay", RELLOG_FILE);

    // parse command line parameters
    if (argc>1) {
	strncpy(command,argv[1],strlen(argv[1])+1);
    } else {
	memset(command,0,256);
    }

    if (!strncasecmp(command,"start",5) ) {
	result = start(RELPID_FILE, WORKDIR);
    } else
    if (!strncasecmp(command,"run",3) ) {
	result = run();
    } else
    if (!strncasecmp(command,"stop",4)) {
	result = stop(RELPID_FILE);
    } else
    if (!strncasecmp(command,"status",6)) {
	result = status();
    } else
    if (!strncasecmp(command,"help",4)) {
	result = help();
    } else
    if (!strncasecmp(command,"install",7)) {
	cout << "Function not available in Linux/Unix systems." << endl;
	result = 0;
    } else
    if (!strncasecmp(command,"uninstall",9)) {
	cout << "Function not available in Linux/Unix systems." << endl;
	result = 0;
    } else
    {
	help();
    }

    logEnd();

    return result;
}
