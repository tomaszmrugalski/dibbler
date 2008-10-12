/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: dibbler-client.cpp,v 1.25 2008-10-12 14:02:40 thomson Exp $
 *
 */

#include <signal.h>
#include <sys/wait.h>  //CHANGED: the following two headers are added.
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "DHCPClient.h"
#include "Portable.h"
#include "Logger.h"
#include "daemon.h"
#include "ClntCfgMgr.h"

TDHCPClient * ptr;

void signal_handler(int n) {
    Log(Crit) << "Signal received. Shutting down." << LogEnd;
    ptr->stop();
}

// when network linkstate change event is detected, this handler will be called.
void signal_handler1(int n) {
    Log(Notice) << "Network switch off event detected. do Confirmming." << LogEnd;

    // get information regarding updated interfaces and call
    // link_state_changed(int ifindex)
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
    int result = (pid > 0)? 0: -1;

    pid = getRelayPID();
    if (pid==-1) {
	cout << "Dibbler relay : NOT RUNNING." << endl;
    } else {
	cout << "Dibbler relay : RUNNING, pid=" << pid << endl;
    }

    return result;
}

int run() {
    if (!init(CLNTPID_FILE, WORKDIR)) {
	return -1;
    }

    if (lowlevelInit()<0) {
	cout << "Lowlevel init failed:" << error_message() << endl;
	return -1;
    }

    TDHCPClient client(CLNTCONF_FILE);
    ptr = &client;

    if (ptr->isDone()) {
	return -1;
    }

    // connect signals (SIGTERM, SIGINT = shutdown)
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    // SIGUSR1 = link state-change
    signal(SIGUSR1,signal_handler1);

    ptr->run();

    lowlevelExit();

    die(CLNTPID_FILE);

    return 0;
}

int help() {
    cout << "Usage:" << endl;
    cout << " dibbler-client ACTION" << endl
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
    int len;

    logStart("(CLIENT, Linux port)", "Client", CLNTLOG_FILE);

    // parse command line parameters
    if (argc>1) {
	len = strlen(argv[1])+1;
	if (len>255)
	    len = 255;
	strncpy(command,argv[1],len);
    } else {
	memset(command,0,256);
    }

    if (!strncasecmp(command,"start",5) ) {
	result = start(CLNTPID_FILE, WORKDIR);
    } else
    if (!strncasecmp(command,"run",3) ) {
	result = run();
    } else
    if (!strncasecmp(command,"stop",4)) {
	result = stop(CLNTPID_FILE);
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

