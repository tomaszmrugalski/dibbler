/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptDomainName.cpp,v 1.5 2004-03-29 19:10:06 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */

#include "ClntOptDomainName.h"
#include "Portable.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptDomainName::TClntOptDomainName(string domain, TMsg* parent)    :TOptDomainName(domain, parent){}
TClntOptDomainName::TClntOptDomainName(char *buf, int bufsize, TMsg* parent)    :TOptDomainName(buf,bufsize, parent){}
bool TClntOptDomainName::doDuties(){    char buf[200];    strncpy(buf,DomainName.c_str(),199);    domain_add("",0, buf);    return true;}
void TClntOptDomainName::setSrvDuid(SmartPtr<TDUID> duid){    this->SrvDUID=duid;}
