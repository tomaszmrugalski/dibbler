/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: RelOptRelayMsg.cpp,v 1.1 2005-01-11 22:53:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "RelOptRelayMsg.h"

TRelOptRelayMsg::TRelOptRelayMsg(char* buf, int bufsize, TMsg* parent) 
    :TOptGeneric(OPTION_RELAY_MSG, buf, bufsize, parent){
}

