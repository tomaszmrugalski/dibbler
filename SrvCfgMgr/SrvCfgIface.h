/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgIface.h,v 1.6 2004-09-03 23:20:23 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 *                                                                           
 */

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

    void setName(string ifaceName);
    void setID(int ifaceID);
    int	getID();
    string getName();

    void addAddrClass(SmartPtr<TSrvCfgAddrClass> addrClass);
    void firstAddrClass();
    SmartPtr<TSrvCfgAddrClass> getAddrClass();
    SmartPtr<TSrvCfgAddrClass> getClassByID(unsigned long id);
    SmartPtr<TSrvCfgAddrClass> getRandomClass(SmartPtr<TDUID> clntDuid, 
					      SmartPtr<TIPv6Addr> clntAddr);
    long countAddrClass();

    void setNoConfig();

    void setOptions(SmartPtr<TSrvParsGlobalOpt> opt);
    virtual ~TSrvCfgIface();
    
    TContainer<SmartPtr<TIPv6Addr> > getDNSSrvLst();
    TContainer<SmartPtr<TIPv6Addr> > getNTPSrvLst();
    string getDomain();
    string getTimeZone();
    unsigned char getPreference();

    bool getRapidCommit();

    long getIfaceMaxLease();
    unsigned long getClntMaxLease();

    // assigned address functions
    void addClntAddr(SmartPtr<TIPv6Addr> ptrAddr);
    void delClntAddr(SmartPtr<TIPv6Addr> ptrAddr);

private:
    unsigned char preference;
    int	ID;
    string Name;
    bool NoConfig;
    bool isUniAddress;
    SmartPtr<TIPv6Addr> UniAddress;
    unsigned long IfaceMaxLease;
    unsigned long ClntMaxLease;
    
    List(TSrvCfgAddrClass) SrvCfgAddrClassLst;
    List(TIPv6Addr) DNSSrv;
    List(TIPv6Addr) NTPSrv;    

    string Domain;
    string TimeZone;

    bool Unicast;		
    bool RapidCommit;	

};

#endif /* SRVCONFIFACE_H */
