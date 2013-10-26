/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include <string.h>
#include "Portable.h"
#include "OptOptionRequest.h"
#include "DHCPConst.h"
#include "Logger.h"

TOptOptionRequest::TOptOptionRequest(uint16_t code, TMsg* parent)
    :TOpt(code, parent), Valid_(true)
{
    Options = NULL;
    OptCnt = 0;
}

int  TOptOptionRequest::getReqOpt(int optNr) {
    if ( (!OptCnt) || (optNr>OptCnt) )
      return 0;
    return
      this->Options[optNr];
}

size_t TOptOptionRequest::getSize()
{
    if (!OptCnt)
        return 0;
    int mySize = 4+(OptCnt<<1);
    return mySize+getSubOptSize();
}

char * TOptOptionRequest::storeSelf( char* buf)
{
    if (!OptCnt)
        return buf;
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize()-4);
    int i=0;
    while(i<OptCnt)
    {
        buf = writeUint16(buf, Options[i]);
        i++;
    }
    return buf;
}

TOptOptionRequest::TOptOptionRequest(uint16_t code, const char * buf, size_t bufSize, TMsg* parent)
    :TOpt(code, parent), Valid_(false)
{
    if (bufSize%2) {
        Log(Error) << "OPTION REQUEST option malformed: odd number of bytes (" << bufSize << ")." << LogEnd;
        Options = NULL;
        OptCnt  = 0;
        return;
    }
    int totalOpts = bufSize/2;
    Options = new unsigned short[totalOpts]; // allocate memory for all options

    for (int i = 0; i < totalOpts; i++) {
        Options[i] = readUint16(buf + i*2);
    }
    OptCnt = totalOpts;
    Valid_ = true;
}

void TOptOptionRequest::addOption(unsigned short optNr)
{
    //Is option already included
    if (isOption(optNr))
        return; //if it is no need to include once more
    //store for a while old options
    unsigned short *oldOptions = Options;
    //assign memort for additional option
    Options = new unsigned short[++OptCnt];
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

bool TOptOptionRequest::isValid() const {
    return Valid_;
}

TOptOptionRequest::~TOptOptionRequest()
{
  if (Options)
    delete [] Options;
}
