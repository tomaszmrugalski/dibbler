/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "DHCPRelay.h"
#include "Portable.h"
#include "Logger.h"
#include "daemon.h"

using namespace std;

TDHCPRelay * ptr = 0;

char *WORKDIR = (char*) "/var/lib/dibbler";

void signal_handler(int n) {
    Log(Crit) << "Signal received. Shutting down." << LogEnd;
    ptr->stop();
}

int status() {
    pid_t pid = getServerPID();
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

    return (pid>0)? 0: -1;
}

int run() {
    if (!init(RELPID_FILE, WORKDIR)) {
	die(RELPID_FILE);
	return -1;
    }

    TDHCPRelay relay(RELCONF_FILE);
    
    ptr = &relay;
    if (ptr->isDone()) {
	die(RELPID_FILE);
	return -1;
    }
    
    // connect signals
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    ptr->run();
    
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

    logStart("(RELAY, Linux port)", "Relay", RELLOG_FILE);

    // parse command line parameters
    if (argc>1) {
	int len = strlen(argv[1])+1;
	if (len > 255)
	    len = 255;
	strncpy(command, argv[1], len);
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

    return result? EXIT_FAILURE: EXIT_SUCCESS;
}
