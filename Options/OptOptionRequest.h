/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef OPTOPTIONREQUEST_H
#define OPTOPTIONREQUEST_H

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Opt.h"
class TClntConfMgr;

class TOptOptionRequest : public TOpt
{
  public:
    TOptOptionRequest(TMsg* parent);
    TOptOptionRequest(char * &buf,  int &bufSize, TMsg* parent);

    void addOption(unsigned short optNr);
    void delOption(unsigned short optNr);
    bool isOption(unsigned short optNr);
    int  count();
    void clearOptions();

     int getSize();
     char * storeSelf( char* buf);
     int  getReqOpt(int optNr);
     bool isValid();
     ~TOptOptionRequest();
  protected:
    bool Valid;
    unsigned short *Options; // you're kidding me, right? Rewrite this ASAP
    int	OptCnt;
};

#endif
