/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptDomainName.cpp,v 1.3 2004-04-11 18:10:56 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */

#include <stdlib.h>
#ifdef LINUX
#include <netinet/in.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#endif
#include "OptDomainName.h"
#include "DHCPConst.h"

using namespace std;

TOptDomainName::TOptDomainName(string DomainName, TMsg* parent)
    :TOpt(OPTION_DOMAIN_LIST, parent)
{
    this->DomainName=DomainName;        
}

TOptDomainName::TOptDomainName(char *&buf, int &bufsize, TMsg* parent)
    :TOpt(OPTION_DOMAIN_LIST, parent)
{
   char* str=new char[bufsize+1];
   memcpy(str,buf+1,bufsize);
   str[bufsize]=0;
   DomainName=string(str);
   delete [] str;
   bufsize-=DomainName.length();
   buf+=DomainName.length();
}

char * TOptDomainName::storeSelf(char* buf)
{
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(getSize()-4);
    buf+=2;
    *buf = DomainName.length()+1;
    buf++;
    memcpy(buf,this->DomainName.c_str(),DomainName.length());
    buf[DomainName.length()+1]=0;
    return buf+DomainName.length()+1;
}

int TOptDomainName::getSize()
{
    return DomainName.length()+7;
}
string TOptDomainName::getDomainName()
{
    return this->DomainName;
}

void TOptDomainName::setDomainName(string name)
{
    this->DomainName=name;
}

