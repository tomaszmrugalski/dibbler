/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 * changes: Paul Schauer <p.schauer(at)comcast(dot)net>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: daemon.h,v 1.3 2009-04-19 21:37:44 thomson Exp $
 *
 */

#ifndef DAEMON_H
#define DAEMON_H

#ifndef SIGTERM
#define SIGTERM 15
#endif

#ifndef SIGINT
#define SIGINT 2
#endif

int start(const char * pidfile, const char * workdir);
int stop(const char * pidfile);

int init(const char * pidfile, const char * workdir);
int getPID(char * pidfile);
int getServerPID();
int getClientPID();
int getRelayPID();
int die(const char * pidfile);

void logStart(const char * note, const char * logname, const char * logfile);
void logEnd();

#endif
