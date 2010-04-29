/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *  changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                        
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: SrvMsgSolicit.h,v 1.6 2008-08-29 00:07:35 thomson Exp $
 *
 */

class TSrvMsgSolicit;
#ifndef SRVMSGSOLICIT_H
#define SRVMSGSOLICIT_H
#include "SrvMsg.h"
#include "SmartPtr.h"

class TSrvMsgSolicit : public TSrvMsg
{
 public:
    TSrvMsgSolicit(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSzie);

    void doDuties();
    void sortAnswers();
    string getName();
    unsigned long getTimeout();
    bool check();
    ~TSrvMsgSolicit();

 private:
    void setAttribs(int iface, char* addr, int msgType);
};
#endif /* SRVMSGSOLICIT_H*/
