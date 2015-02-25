/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <signal.h>
#include <sys/stat.h>
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

TDHCPClient * ptr;
//static const char *TOOL_NAME = "ifplugstatus";

char *WORKDIR = (char*) "/var/lib/dibbler";
char *CLNTCONF_FILE = (char*) "/etc/dibbler/client.conf";
char *CLNTLOG_FILE = (char*) "/var/log/dibbler/dibbler-client.log";
extern char CLNT_LLAADDR[];

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
    
    chmod(CLNTPID_FILE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    
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
         << " OPTION = -W <filepath> -A <LLA_ADDR>" << endl
         << " -W <filepath> - specify the client's working directory." << endl
         << " -A <LLA_ADDR> - specify the client's srouce LLA address." << endl
	 << " help      - displays usage info." << endl;
    return 0;
}

int main(int argc, char * argv[])
{
    char command[256];
    int result = -1;
    bool default_workdir = true;

    // parse command line parameters
    memset(command,0,256);
    if (argc>1) {
	int len = strlen(argv[1])+1;
        int c;
        if (len>255)
            len = 255;
        strncpy(command,argv[1],len);

        while ((c = getopt(argc-1, argv + 1, "W:A:")) != -1)
          switch (c)
            {
            case 'W':
                default_workdir = false;
                len = strlen(optarg) + 1;
                WORKDIR = (char *) calloc(len, 1);
                strncpy(WORKDIR,optarg,len);

                len = strlen(WORKDIR) + strlen("/client.pid") + 1;
                CLNTPID_FILE = (char *) calloc(len, 1);
                sprintf(CLNTPID_FILE, "%s/client.pid", WORKDIR);

                len = strlen(WORKDIR) + strlen("/client.conf") + 1;
                CLNTCONF_FILE = (char *) calloc(len, 1);
                sprintf(CLNTCONF_FILE, "%s/client.conf", WORKDIR);

                len = strlen(WORKDIR) + strlen("/client.log") + 1;
                CLNTLOG_FILE = (char *) calloc(len, 1);
                sprintf(CLNTLOG_FILE, "%s/client.log", WORKDIR);
                break;
            case 'A':
                char addr[16];
                if (inet_pton6(optarg, addr))
                    strcpy(CLNT_LLAADDR, optarg);
                else {
	            cout << "Invalid IPv6 address: " << optarg << endl;
                    help();
                    goto exit_failure;
                }
                break;
            default:
                help();
                goto exit_failure;
            }
    }

    logStart("(CLIENT, Linux port)", "Client", CLNTLOG_FILE);

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

exit_failure:
    if (!default_workdir) {
        free(WORKDIR);
        free(CLNTPID_FILE);
        free(CLNTCONF_FILE);
        free(CLNTLOG_FILE);
    }
    return result? EXIT_FAILURE: EXIT_SUCCESS;
}

