/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <signal.h>
#include <sys/wait.h>  //CHANGED: the following two headers are added.
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "DHCPClient.h"
#include "Portable.h"
#include "Logger.h"
#include "daemon.h"
#include "ClntCfgMgr.h"
#include <map>
#include <pthread.h>

using namespace std;
using std::map;

#define IF_RECONNECTED_DETECTED -1

extern pthread_mutex_t lock;

char *CLNTCONF_FILE = "/etc/dibbler/client.conf";
char *CLNTPID_FILE = "/var/lib/dibbler/client.pid";

TDHCPClient * ptr;
//static const char *TOOL_NAME = "ifplugstatus";


void signal_handler(int n) {
    Log(Crit) << "Signal received. Shutting down." << LogEnd;
    ptr->stop();
}


#ifdef MOD_CLNT_CONFIRM
void signal_handler_of_linkstate_change(int n) {
    Log(Notice) << "Network switch off event detected. initiating CONFIRM." << LogEnd;
    pthread_mutex_lock(&lock);
    pthread_mutex_unlock(&lock);
}
#endif

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
	die(CLNTPID_FILE);
	return -1;
    }

    if (lowlevelInit()<0) {
	cout << "Lowlevel init failed:" << error_message() << endl;
	die(CLNTPID_FILE);
	return -1;
    }

    TDHCPClient client(CLNTCONF_FILE);
    ptr = &client;

    if (ptr->isDone()) {
	die(CLNTPID_FILE);
	return -1;
    }

    // connect signals (SIGTERM, SIGINT = shutdown)
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

#ifdef MOD_CLNT_CONFIRM
    // CHANGED: add a signal handling function to handle SIGUSR1,
    // which will be generated if network switch off.
    // SIGUSR1 = link state-change
    signal(SIGUSR1, signal_handler_of_linkstate_change);
    Log(Notice) << "CONFIRM support compiled in." << LogEnd;
#else
    Log(Info) << "CONFIRM support not compiled in." << LogEnd;
#endif

    ptr->run();

    lowlevelExit();

    die(CLNTPID_FILE);

    return 0;
}

int help() {
    cout << "Usage:" << endl;
    cout << " dibbler-client ACTION OPTION" << endl
	 << " ACTION = status|start|stop|install|uninstall|run" << endl
	 << " status    - show status and exit" << endl
	 << " start     - start installed service" << endl
	 << " stop      - stop installed service" << endl
	 << " install   - Not available in Linux/Unix systems." << endl
	 << " uninstall - Not available in Linux/Unix systems." << endl
	 << " run       - run in the console" << endl
	 << " help      - displays usage info." << endl
	 << " OPTIONS: " << endl
	 << " -C FILEPATH - Specify the config file. " << endl
	 << " -P FILEPATH - Specify the PID file. " << endl;
    return 0;
}

int parse_options(std::string option, char* value)
{
    cout << "You've used option: " << option << " with: " << value << endl;
    if (option == "-C") {
        cout << "You passed me a config file!" << endl;
        CLNTCONF_FILE = value;
        cout << "My new config file is: " << CLNTCONF_FILE << endl;
    } else if (option == "-P") {
        cout << "You passed me a PID file!" << endl;
        CLNTPID_FILE = value;
        cout << "My new PID file is: " << CLNTPID_FILE << endl;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int result = -1;

    logStart("(CLIENT, Linux port)", "Client", CLNTLOG_FILE);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "start") {
            if (i + 2 < argc) {
                cout << "You've passed me an argument!" << endl;
                parse_options(argv[i+1],argv[i+2]);
            }
	        result = start(CLNTPID_FILE, WORKDIR);
        } else if (arg == "run") {
            cout << "I'm running!" << endl;
            if (i + 2 < argc) {
                cout << "You've passed me an argument!" << endl;
                parse_options(argv[i+1],argv[i+2]);
            }
            result = run();
        } else if (arg == "stop") {
            result = stop(CLNTPID_FILE);
        } else if (arg == "status") {
            result = status();
        } else if (arg == "help") {
            result = help();
        } else if (arg == "install") {
            cout << "Function not available in Linux/Unix systems." << endl;
            result = 0;
        } else if (arg == "uninstall") {
            cout << "Function not available in Linux/Unix systems." << endl;
            result = 0;
        }
    }

    return result? EXIT_FAILURE: EXIT_SUCCESS;
}

