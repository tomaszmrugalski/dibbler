/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: OptOptionRequest.h,v 1.2 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

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
    int  count();
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
