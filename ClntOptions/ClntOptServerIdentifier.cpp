/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptServerIdentifier.cpp,v 1.4 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/03/29 19:10:06  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */

#include <stdlib.h>
#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "DHCPConst.h"
#include "ClntOptServerIdentifier.h"

TClntOptServerIdentifier::TClntOptServerIdentifier( char * duid,  int n, TMsg* parent)
	:TOptServerIdentifier(duid,n, parent) {

}

TClntOptServerIdentifier::TClntOptServerIdentifier(SmartPtr<TDUID> duid, TMsg* parent)
    :TOptServerIdentifier(duid, parent) {

}

bool TClntOptServerIdentifier::doDuties() {
    return false;
}
