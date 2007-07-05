/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptOptionRequest.cpp,v 1.5 2007-07-05 00:17:41 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2004-10-25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
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
#include "OptOptionRequest.h"
#include "DHCPConst.h"
#include "Logger.h"

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

TOptOptionRequest::TOptOptionRequest( char * &buf,  int &bufSize, TMsg* parent)
	:TOpt(OPTION_ORO, parent)
{
    if (bufSize/2) {
	Log(Error) << "OPTION REQUEST option malformed: odd number of bytes (" << bufSize << ")." << LogEnd;
        Valid=false;
	return;
    }
    int totalOpts = bufSize/2;
    Options = new unsigned short[totalOpts]; // allocate memory for all options

    int i=0;
    for (i=0; i<totalOpts; i++) {
	Options[i] = ntohs(*(unsigned short*) (buf+i*2) );
    }
}

void TOptOptionRequest::addOption(unsigned short optNr)
{
    //Is option already included
    if (isOption(optNr))
        return; //if it is no need to include once more
    //store for a while old options
    unsigned short *oldOptions=Options;
    //assign memort for additional option
    Options=new unsigned short[++OptCnt];
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

void TOptOptionRequest::delOption(unsigned short optNr)
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

bool TOptOptionRequest::isOption(unsigned short optNr)
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
