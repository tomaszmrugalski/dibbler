/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelCfgIface.h,v 1.3 2007-05-01 12:03:13 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2007-03-07 02:37:10  thomson
 * Experimental support for guess-mode.
 *
 * Revision 1.1  2005-01-11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

class TRelCfgIface;

#ifndef RELCFGIFACE_H
#define RELCFGIFACE_H
#include "DHCPConst.h"
#include "RelParsGlobalOpt.h"
#include <iostream>
#include <string>
using namespace std;

class TRelCfgIface
{
    friend ostream& operator<<(ostream& out,TRelCfgIface& iface);
public:
    TRelCfgIface(string ifaceName);
    TRelCfgIface(int ifaceNr);
    virtual ~TRelCfgIface();
    void setDefaults();

    void setName(string ifaceName);
    void setID(int ifaceID);
    int	getID();
    string getName();
    string getFullName();

    SmartPtr<TIPv6Addr> getServerUnicast();
    SmartPtr<TIPv6Addr> getClientUnicast();
    bool getServerMulticast();
    bool getClientMulticast();

    void setOptions(SmartPtr<TRelParsGlobalOpt> opt);
    
    unsigned char getPreference();
    int getInterfaceID();
    
private:
    string Name;
    int	ID;
    int InterfaceID; // value of interface-id option (optional)

    SmartPtr<TIPv6Addr> ClientUnicast;
    SmartPtr<TIPv6Addr> ServerUnicast;
    bool ClientMulticast;
    bool ServerMulticast;
};

#endif /* RELCFGIFACE_H */
