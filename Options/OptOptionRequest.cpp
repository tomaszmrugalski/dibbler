/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptOptionRequest.cpp,v 1.4 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/04/11 18:10:56  thomson
 * CRLF fixed.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */
#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "SmartPtr.h"
//#include "ClntCfgMgr.h"
#include "OptOptionRequest.h"
#include "DHCPConst.h"

TOptOptionRequest::TOptOptionRequest(TMsg* parent)
	:TOpt(OPTION_ORO, parent)
{
    //FIXME: appropriate options should be set in parent constructor
    this->Options=NULL;
    this->OptCnt=0;
}

int  TOptOptionRequest::getReqOpt(int optNr) {
    if ( (!OptCnt) || (optNr>OptCnt) )
	return 0;
    return this->Options[optNr];
}

 int TOptOptionRequest::getSize()
{
	if (!OptCnt) 
        return 0;
	int mySize = 4+(OptCnt<<1);
	return mySize+getSubOptSize();
}

char * TOptOptionRequest::storeSelf( char* buf)
{
    if (!OptCnt) return buf;
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons( getSize()-4 );
    buf+=2;
    int i=0;
    while(i<OptCnt)
    {
        *(uint16_t*)buf = htons( Options[i] );
        buf+=2;
        i++;
    }
    return buf;
}

TOptOptionRequest::TOptOptionRequest( char * &buf,  int &n, TMsg* parent)
	:TOpt(OPTION_ORO, parent)
{
	Options = new short[n>>1];
	int pos=0;
	while (pos<(n>>1))
    {
        Options[pos]=ntohs(*(short*)buf);
        buf+=2;
        pos++;
    }
	OptCnt=n>>1;
    if (n%2)
        Valid=false;
    else
        Valid=true;
}

void TOptOptionRequest::addOption(short optNr)
{
    //Is option already included
    if (isOption(optNr))
        return; //if it is no need to include once more
    //store for a while old options
    short *oldOptions=Options;
    //assign memort for additional option
    Options=new short[++OptCnt];
    //If there were options before
    if (oldOptions)
    {
        //copy them to new memory
        memcpy(Options,oldOptions,(OptCnt-1)*sizeof(short));
        //now they can be deleted
        delete [] oldOptions;
    }
    //and add new option
    Options[OptCnt-1]=optNr;
}

void TOptOptionRequest::delOption(short optNr)
{
    //find option if any
    if (!OptCnt) return;
    int optIdx=0;
    while((optIdx<OptCnt)&&(Options[optIdx]!=optNr)) 
        optIdx++;
    //if there is no such a option
    if (optIdx>=OptCnt) return;
    memcpy(Options+optIdx,Options+optIdx+1,(OptCnt-optIdx-1)<<1);
    OptCnt--;
}

bool TOptOptionRequest::isOption(short optNr)
{
    for(int i=0;i<OptCnt;i++)
        if (Options[i]==optNr)
            return true;
    return false;
}

int  TOptOptionRequest::count() {
    return this->OptCnt;
}

void TOptOptionRequest::clearOptions() {
    if (this->OptCnt)
        delete [] this->Options;
    OptCnt=0;
}

bool TOptOptionRequest::isValid() {
    return this->Valid;
}

TOptOptionRequest::~TOptOptionRequest()
{
    delete [] Options;
}
