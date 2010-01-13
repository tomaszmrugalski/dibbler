/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelParsIfaceOpt.h,v 1.3 2008-08-29 00:07:32 thomson Exp $
 *
 */

#ifndef RELPARSIFACEOPT_H_
#define RELPARSIFACEOPT_H_

#include "SmartPtr.h"
#include "IPv6Addr.h"

class TRelParsIfaceOpt
{
public:
    TRelParsIfaceOpt(void);
    ~TRelParsIfaceOpt(void);

    void setClientUnicast(SPtr<TIPv6Addr> addr);
    void setServerUnicast(SPtr<TIPv6Addr> addr);
    void setClientMulticast(bool unicast);
    void setServerMulticast(bool unicast);

    SPtr<TIPv6Addr> getServerUnicast();
    SPtr<TIPv6Addr> getClientUnicast();
    bool getServerMulticast();
    bool getClientMulticast();

    void setInterfaceID(int id);
    int getInterfaceID();

private:
    SPtr<TIPv6Addr> ClientUnicast;
    SPtr<TIPv6Addr> ServerUnicast;
    bool ClientMulticast;
    bool ServerMulticast;

    int InterfaceID;
};

#endif
