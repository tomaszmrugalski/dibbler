/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: ReqCfgMgr.h,v 1.3 2007-12-08 04:14:03 thomson Exp $
 */

#ifndef REQCFGMGR_H
#define REQCFGMGR_H

typedef struct {
    // global parameters
    char * iface;
    int timeout;
    
    char * dstaddr;

    // message specific parameters
    char * addr;
    char * duid;
    char * bulk;
} ReqCfgMgr;

#endif
