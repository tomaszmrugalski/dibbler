/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: daemon.h,v 1.1 2005-02-03 22:06:40 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef DAEMON_H
#define DAEMON_H

#ifndef SIGTERM
#define SIGTERM 15
#endif

#ifndef SIGINT
#define SIGINT 2
#endif

int start(char * pidfile, char * workdir);
int stop(char * pidfile);

int init(char * pidfile, char * workdir);
int getPID(char * pidfile);
int getServerPID();
int getClientPID();
int getRelayPID();
int die(char * pidfile);

void logStart(char * note, char * logname, char * logfile);
void logEnd();

#endif
