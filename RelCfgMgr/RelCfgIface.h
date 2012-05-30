/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TRelCfgIface;

#ifndef RELCFGIFACE_H
#define RELCFGIFACE_H
#include "DHCPConst.h"
#include "RelParsGlobalOpt.h"
#include <iostream>
#include <string>

class TRelCfgIface
{
    friend std::ostream& operator<<(std::ostream& out,TRelCfgIface& iface);
public:
    TRelCfgIface(const std::string& ifaceName);
    TRelCfgIface(int ifaceNr);
    virtual ~TRelCfgIface();

    void setName(std::string ifaceName);
    void setID(int ifaceID);
    int	getID();
    std::string getName();
    std::string getFullName();

    SPtr<TIPv6Addr> getServerUnicast();
    SPtr<TIPv6Addr> getClientUnicast();
    bool getServerMulticast();
    bool getClientMulticast();

    void setOptions(SPtr<TRelParsGlobalOpt> opt);
    
    unsigned char getPreference();
    int getInterfaceID();
    
private:
    std::string Name_;
    int	ID_;
    int InterfaceID_; // value of interface-id option (optional)

    SPtr<TIPv6Addr> ClientUnicast_;
    SPtr<TIPv6Addr> ServerUnicast_;
    bool ClientMulticast_;
    bool ServerMulticast_;
};

#endif /* RELCFGIFACE_H */
