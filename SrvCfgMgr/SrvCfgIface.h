class TSrvCfgIface;
#ifndef SRVCONFIFACE_H
#define SRVCONFIFACE_H
#include "DHCPConst.h"
#include "SrvCfgAddrClass.h"
#include "SrvParsGlobalOpt.h"
#include <iostream>
#include <string>
using namespace std;

class TSrvCfgIface
{
    friend ostream& operator<<(ostream& out,TSrvCfgIface& iface);
public:
    TSrvCfgIface();
    TSrvCfgIface(string ifaceName);
    TSrvCfgIface(int ifaceNr);

    void setIfaceName(string ifaceName);
    void setIfaceID(int ifaceID);
    int	getID();
    string getName();

    void addAddrClass(SmartPtr<TSrvCfgAddrClass> addrClass);
    void firstAddrClass();   
    SmartPtr<TSrvCfgAddrClass> getAddrClass();
    SmartPtr<TSrvCfgAddrClass> getRandomClass(SmartPtr<TDUID> clntDuid, 
					      SmartPtr<TIPv6Addr> clntAddr);
    long countAddrClass();

    void	setNoConfig();

    void	setOptions(SmartPtr<TSrvParsGlobalOpt> opt);
    virtual ~TSrvCfgIface();
    
    TContainer<SmartPtr<TIPv6Addr> > getDNSSrvLst();
    TContainer<SmartPtr<TIPv6Addr> > getNTPSrvLst();
    string getDomain();
    string getTimeZone();
    unsigned char getPreference();

private:
    unsigned char preference;
    int		ID;
    string	Name;
    bool	NoConfig;
    bool	isUniAddress;
    SmartPtr<TIPv6Addr> UniAddress;
    
    TContainer <SmartPtr<TSrvCfgAddrClass> > SrvCfgAddrClassLst;
    TContainer<SmartPtr<TIPv6Addr> > DNSSrv;
    TContainer<SmartPtr<TIPv6Addr> > NTPSrv;    

    string				Domain;
    string				TimeZone;

};

#endif /* SRVCONFIFACE_H_HEADER_INCLUDED_C100F216 */
