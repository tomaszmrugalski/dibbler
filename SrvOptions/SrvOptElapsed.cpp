/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptElapsed.cpp,v 1.3 2006-01-29 20:03:20 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 */

#include "DHCPConst.h"
#include "SrvOptElapsed.h"

TSrvOptElapsed::TSrvOptElapsed( char * buf,  int n, TMsg* parent)
    :TOptInteger4(OPTION_ELAPSED_TIME, buf,n, parent) {
    
}

TSrvOptElapsed::TSrvOptElapsed(TMsg* parent)
    :TOptInteger4(OPTION_ELAPSED_TIME, 0, parent) {

}

bool TSrvOptElapsed::doDuties() {
    return true;
}
