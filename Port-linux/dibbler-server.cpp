/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: dibbler-server.cpp,v 1.18 2005-10-11 20:52:47 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.17  2005/07/31 14:39:40  thomson
 * Minor changes related to 0.4.1 release.
 *
 * Revision 1.16  2005/07/21 21:40:19  thomson
 * PID checking process is improved.
 *
 * Revision 1.15  2005/04/29 00:08:20  thomson
 * *** empty log message ***
 *
 * Revision 1.14  2005/02/03 22:06:40  thomson
 * Linux startup/pid checking changed.
 *
 * Revision 1.13  2005/02/03 20:09:11  thomson
 * *** empty log message ***
 *
 * Revision 1.12  2005/01/30 23:12:28  thomson
 * *** empty log message ***
 *
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

#include <signal.h>
#include "DHCPServer.h"
#include "Portable.h"
#include "Logger.h"
#include "daemon.h"

TDHCPServer * ptr;

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
    int result = pid;
    
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
    if (!init(SRVPID_FILE, WORKDIR)) {
	return -1;
    }

    TDHCPServer srv(SRVCONF_FILE);
    
    ptr = &srv;
    
    // connect signals
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    
    ptr->run();

    die(SRVPID_FILE);
    return 0;
}


int help() {
    cout << "Usage:" << endl;
    cout << " dibbler-server ACTION" << endl
	 << " ACTION = status|start|stop|run" << endl
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

    logStart("(SERVER, Linux port)", "Server", SRVLOG_FILE);

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
	result = start(SRVPID_FILE, WORKDIR);
    } else
    if (!strncasecmp(command,"run",3) ) {
	result = run();
    } else
    if (!strncasecmp(command,"stop",4)) {
	result = stop(SRVPID_FILE);
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
