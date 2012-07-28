/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptTA.h,v 1.4 2008-08-29 00:07:29 thomson Exp $
 *
 */

class TOptTA;
class TMsg;

#ifndef CLNTIA_TA_H
#define CLNTIA_TA_H

#include "OptTA.h"
#include "ClntOptIAAddress.h"
#include "IPv6Addr.h"
#include "Msg.h"

class TClntOptTA : public TOptTA
{
 public:
    TClntOptTA(unsigned int iaid, TMsg* parent);
    TClntOptTA(char * buf, int bufsize, TMsg* parent);
    TClntOptTA(SPtr<TAddrIA> ta, TMsg* parent);
    ~TClntOptTA();    

    bool doDuties();
    int getStatusCode();

    SPtr<TClntOptIAAddress> getAddr();
    SPtr<TClntOptIAAddress> getAddr(SPtr<TIPv6Addr> addr);
    void firstAddr();
    int countAddr();
    bool isValid();
    void setIface(int iface); // used to override interface (e.g. when msg is received via loopback)

    void setContext(int iface, SPtr<TIPv6Addr> clntAddr);

 private:
    void releaseAddr(long IAID, SPtr<TIPv6Addr> addr );

    SPtr<TIPv6Addr> Addr;
    int Iface;
};


#endif /* CLNTIA_TA_H */
