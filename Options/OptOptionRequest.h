#ifndef OPTOPTIONREQUEST_H
#define OPTOPTIONREQUEST_H

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Opt.h"
#include "OptClientIdentifier.h"

class TClntConfMgr;

class TOptOptionRequest : public TOpt
{
  public:
    TOptOptionRequest(TMsg* parent);
    TOptOptionRequest(char * &buf,  int &n, TMsg* parent);
    
    void addOption(short optNr);
    void delOption(short optNr);
    bool isOption(short optNr);
    int  getOptCnt();
    void clearOptions();

     int getSize();
     char * storeSelf( char* buf);
     int  getReqOpt(int optNr);
     bool isValid();
     ~TOptOptionRequest();	
  protected:
    bool Valid;
    short *Options;
    int	OptCnt;
};

#endif
