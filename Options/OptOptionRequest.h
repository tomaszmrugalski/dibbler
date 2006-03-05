/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: OptOptionRequest.h,v 1.4 2006-03-05 21:37:46 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3.2.1  2006/02/05 23:38:08  thomson
 * Devel branch with Temporary addresses support added.
 *
 * Revision 1.3  2006/02/02 23:17:17  thomson
 * 0.4.2 release.
 *
 * Revision 1.2  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
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
