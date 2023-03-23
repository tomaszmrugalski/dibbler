/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "ClntCfgMgr.h"
#include "DHCPClient.h"
#include "Logger.h"
#include "Portable.h"
#include "daemon.h"
#include <errno.h>
#include <map>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

#define IF_RECONNECTED_DETECTED -1

extern pthread_mutex_t dibbler_lock;

TDHCPClient *ptr = 0;

std::string WORKDIR(DEFAULT_WORKDIR);
std::string CLNTCONF_FILE(DEFAULT_CLNTCONF_FILE);
std::string CLNTLOG_FILE(DEFAULT_CLNTLOG_FILE);
std::string CLNTPID_FILE(DEFAULT_CLNTPID_FILE);

void signal_handler(int n) {
  Log(Crit) << "Signal received. Shutting down." << LogEnd;
  ptr->stop();
}

#ifdef MOD_CLNT_CONFIRM
void signal_handler_of_linkstate_change(int n) {
  Log(Notice) << "Network switch off event detected. initiating CONFIRM." << LogEnd;
  pthread_mutex_lock(&dibbler_lock);
  pthread_mutex_unlock(&dibbler_lock);
}
#endif

int status() {

  pid_t pid = getServerPID();
  if (pid == -1) {
    cout << "Dibbler server: NOT RUNNING." << endl;
  } else {
    cout << "Dibbler server: RUNNING, pid=" << pid << endl;
  }

  pid = getClientPID();
  if (pid == -1) {
    cout << "Dibbler client: NOT RUNNING." << endl;
  } else {
    cout << "Dibbler client: RUNNING, pid=" << pid << endl;
  }
  int result = (pid > 0) ? 0 : -1;

  pid = getRelayPID();
  if (pid == -1) {
    cout << "Dibbler relay : NOT RUNNING." << endl;
  } else {
    cout << "Dibbler relay : RUNNING, pid=" << pid << endl;
  }

  return result;
}

int run() {
  if (!init(CLNTPID_FILE.c_str(), WORKDIR.c_str())) {
    die(CLNTPID_FILE.c_str());
    return -1;
  }

  // Change attribs to the client file: 644 (user=rw, group=r, other=r)
  chmod(CLNTPID_FILE.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  if (lowlevelInit() < 0) {
    cout << "Lowlevel init failed:" << error_message() << endl;
    die(CLNTPID_FILE.c_str());
    return -1;
  }

  TDHCPClient client(CLNTCONF_FILE.c_str());
  ptr = &client;

  if (ptr->isDone()) {
    die(CLNTPID_FILE.c_str());
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

  die(CLNTPID_FILE.c_str());

  return 0;
}

int help() {
  cout << "Usage:" << endl;
  cout << " dibbler-client ACTION [OPTION]" << endl
       << " ACTION = status|start|stop|install|uninstall|run" << endl
       << " status    - show status and exit" << endl
       << " start     - start installed service" << endl
       << " stop      - stop installed service" << endl
       << " install   - Not available in Linux/Unix systems." << endl
       << " uninstall - Not available in Linux/Unix systems." << endl
       << " run       - run in the console" << endl
       << " help      - displays usage info." << endl
       << endl
       << " OPTION = -w <directory>" << endl
       << " -w <directory> - specify the client's working directory." << endl;
  return 0;
}

int main(int argc, char *argv[]) {
  int result = -1;

  // If at least one parameter (command) is not specified, then...
  if (argc < 2) {
    help();
    return EXIT_FAILURE;
  }

  // The first parameter is command
  const char *command = argv[1];

  // Let's parse additional commands if specified
  if (argc > 2) {
    int c;

    // Let's go through any additional command line options.
    while ((c = getopt(argc - 1, argv + 1, "w:")) != -1) {
      switch (c) {
        case 'w':
          WORKDIR = string(optarg);
          CLNTPID_FILE = string(optarg) + "/client.pid";
          CLNTCONF_FILE = string(optarg) + "/client.conf";
          CLNTLOG_FILE = string(optarg) + "/client.log";
          break;
        default:
          help();
          return result ? EXIT_FAILURE : EXIT_SUCCESS;
      }
    }
  }

  logStart("(CLIENT, Linux port)", "Client", CLNTLOG_FILE.c_str());

  if (!strncasecmp(command, "start", 5)) {
    result = start(CLNTPID_FILE.c_str(), WORKDIR.c_str());
  } else if (!strncasecmp(command, "run", 3)) {
    result = run();
  } else if (!strncasecmp(command, "stop", 4)) {
    result = stop(CLNTPID_FILE.c_str());
  } else if (!strncasecmp(command, "status", 6)) {
    result = status();
  } else if (!strncasecmp(command, "help", 4)) {
    result = help();
  } else if (!strncasecmp(command, "install", 7)) {
    cout << "Function not available in Linux/Unix systems." << endl;
    result = 0;
  } else if (!strncasecmp(command, "uninstall", 9)) {
    cout << "Function not available in Linux/Unix systems." << endl;
    result = 0;
  } else {
    help();
  }

  logEnd();

  return result ? EXIT_FAILURE : EXIT_SUCCESS;
}
