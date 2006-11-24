/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Micha³ Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: daemon.h,v 1.2 2006-11-24 01:25:17 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005-02-03 22:06:40  thomson
 * Linux startup/pid checking changed.
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
