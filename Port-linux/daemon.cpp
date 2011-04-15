/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 licence
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Portable.h"
#include "Logger.h"

extern int status();
extern int run();

using namespace std;

/** 
 * checks if pid file exists, and returns its content (or -2 if unable to read)
 * 
 * @param file 
 * 
 * @return pid value, or negative if error was detected
 */
pid_t getPID(const char * file) {
    /* check if the file exists */
    struct stat buf;
    int i = stat(file, &buf);
    if (i!=0)
	return LOWLEVEL_ERROR_UNSPEC;

    ifstream pidfile(file);
    if (!pidfile.is_open()) 
	return LOWLEVEL_ERROR_FILE;
    pid_t pid;
    pidfile >> pid;
    return pid;
}

pid_t getClientPID() {
    return getPID(CLNTPID_FILE);
}

pid_t getServerPID() {
    return getPID(SRVPID_FILE);
}

pid_t getRelayPID() {
    return getPID(RELPID_FILE);
}

void daemon_init() {
    std::cout << "Starting daemon..." << std::endl;

    // daemon should close all open files
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);

    pid_t childpid;
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

    umask(DEFAULT_UMASK);
}

void daemon_die() {
    logger::Terminate();
    logger::EchoOn();
}

int init(const char * pidfile, const char * workdir) {
    string tmp;
    /*FIXME: buf needs to fit "/proc/%d/exe", where %d is pid_t
     * (on my system it's 20 B exactly with positive PID. However this is not
     * portable.) */
    char buf[20];
    char cmd[256];
    pid_t pid = getPID(pidfile);
    if (pid > 0) {
	/* XXX: ISO C++ doesn't support 'j' length modifier nor 'll' nor
	 * PRIdMAX macro. So, long int is the biggest printable type.
	 * God bless pid_t to fit into long int. */
	if (snprintf(buf, sizeof(buf), "/proc/%ld", (long int)pid)
		>= (int)sizeof(buf)) {
	    Log(Crit) << "Buffer for string `/proc/" << pid << "' too small"
		      << LogEnd;
	    return 0;
	};
	if (!access(buf, F_OK)) {
	    if(snprintf(buf, sizeof(buf), "/proc/%ld/exe", (long int)pid)
		    >= (int)sizeof(buf)) {
		Log(Crit) << "Buffer for string `/proc/" << pid << "/exe' too small"
			  << LogEnd;
		return 0;
	    }

	    ssize_t len=readlink(buf, cmd, sizeof(cmd));
	    if(len!=-1) {
	        cmd[len]=0;
		if(strstr(cmd, "dibbler")==NULL) {
		    Log(Warning) << "Process is running but it is not Dibbler (pid=" << pid
				 << ", name " << cmd << ")." << LogEnd;
		} else {
    		    Log(Crit) << "Process already running and it seems to be Dibbler (pid=" 
			      << pid << ", name " << cmd << ")." << LogEnd;
		    return 0;
		}
	    } else {
    		Log(Crit) << "Process already running (pid=" << pid << ", file " << pidfile 
		    	  << " is present)." << LogEnd;
		return 0;
	    }
	} else {
	    Log(Warning) << "Pid file found (pid=" << pid << ", file " << pidfile 
			 << "), but process " << pid << " does not exist." << LogEnd;
	}
    }

    unlink(pidfile);
    ofstream pidFile(pidfile);
    if (!pidFile.is_open()) {
	Log(Crit) << "Unable to create " << pidfile << " file." << LogEnd;
	return 0;
    }
    pidFile << getpid();
    pidFile.close();
    Log(Notice) << "My pid (" << getpid() << ") is stored in " << pidfile << LogEnd;

    if (chdir(workdir)) {
	Log(Crit) << "Unable to change directory to " << workdir << "." << LogEnd;
	return 0;
    }

    umask(DEFAULT_UMASK);

    return 1;
}

void die(const char * pidfile) {
    if (unlink(pidfile)) {
	Log(Warning) << "Unable to delete " << pidfile << "." << LogEnd;
    }
}



int start(const char * pidfile, const char * workdir) {
    int result;
    daemon_init();
    result = run();
    daemon_die();
    return result;
}

int stop(const char * pidfile) {
    int saved_errno;
    int ptrace_failed, p_status;

    pid_t pid = getPID(pidfile);
    if (pid==-1) {
	cout << "Process is not running." << endl;
	return -1;
    }
    if (pid==-2) {
	cout << "Unable to read file " << pidfile << ". Are you running as root?" << endl;
	return -1;
    }

    ptrace_failed = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    if (ptrace_failed) {
	saved_errno = errno;
	cout << "Attaching to process " << pid << " failed: "
	     << strerror(saved_errno) << endl;
	cout << "Warning: Can not guarantee for remote process termination" << endl;
    }

    cout << "Sending TERM signal to process " << pid << endl;
    if (-1 == kill(pid, SIGTERM)) {
	saved_errno = errno;
	cout << "Signal sending failed: " << strerror(saved_errno) << endl;
	if (!ptrace_failed) ptrace(PTRACE_DETACH, pid, NULL, NULL);
	return -1;
    }

    if (!ptrace_failed) {
	cout << "Waiting for signalled process termination... " << flush;
	do {
	    if (-1 == waitpid(pid, &p_status, 0)) {
		saved_errno = errno;
		cout << "Failed: " << strerror(saved_errno) << endl;
		ptrace(PTRACE_DETACH, pid, NULL, NULL);
		return -1;
	    }
	    ptrace(PTRACE_CONT, pid, NULL,
		    WIFSTOPPED(p_status) ? WSTOPSIG(p_status) : NULL);
	} while (! (WIFEXITED(p_status) || WIFSIGNALED(p_status)) );
	cout << "Done." << endl;
    }

    return 0;
}

int install() {
    return 0;
}

int uninstall() {
    return 0;
}

/** things to do just after started */
void logStart(const char * note, const char * logname, const char * logfile) {
    std::cout << DIBBLER_COPYRIGHT1 << " " << note << std::endl;
    std::cout << DIBBLER_COPYRIGHT2 << std::endl;
    std::cout << DIBBLER_COPYRIGHT3 << std::endl;
    std::cout << DIBBLER_COPYRIGHT4 << std::endl;

    logger::setLogName(logname);
    logger::Initialize(logfile);

    logger::EchoOff();
    Log(Notice) << DIBBLER_COPYRIGHT1 << " " << note << LogEnd;
    logger::EchoOn();
}

/** things to do just before end 
 */
void logEnd() {
    logger::Terminate();
}
