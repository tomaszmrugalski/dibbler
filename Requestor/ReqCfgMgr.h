/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: ReqCfgMgr.h,v 1.1 2007-12-02 10:31:59 thomson Exp $
 */

#ifndef REQCFGMGR_H
#define REQCFGMGR_H

typedef struct {
    // global parameters
    char * iface;
    int timeout;

    // message specific parameters
    char * addr;
    char * duid;
} ReqCfgMgr;

#endif