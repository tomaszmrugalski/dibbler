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
    TOptOptionRequest(uint16_t code, TMsg* parent);
    TOptOptionRequest(uint16_t code, const char * buf, size_t size, TMsg* parent);

    void addOption(unsigned short optNr);
    void delOption(unsigned short optNr);
    bool isOption(unsigned short optNr);
    int  count();
    void clearOptions();

    size_t getSize();
    char * storeSelf( char* buf);
    int getReqOpt(int optNr);
    virtual bool isValid() const;
    bool doDuties() { return true; }
    ~TOptOptionRequest();
  protected:
    bool Valid_;
    unsigned short *Options; /// @todo: you're kidding me, right? Rewrite this ASAP
    int	OptCnt;
};

#endif
