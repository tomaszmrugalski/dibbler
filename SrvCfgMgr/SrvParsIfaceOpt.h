/*
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvParsIfaceOpt.h,v 1.5 2004-09-03 23:20:23 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 * Revision 1.3  2004/06/28 22:37:59  thomson
 * Minor changes.
 *
 */

#ifndef TSRCPARSIFACEOPT_H_
#define TSRCPARSIFACEOPT_H_
#include "SrvParsClassOpt.h"

class TSrvParsIfaceOpt : public TSrvParsClassOpt
{
public:
    TSrvParsIfaceOpt(void);
    ~TSrvParsIfaceOpt(void);

    bool uniAddress();
    void setUniAddress(bool isUni);

    void setAddress(SmartPtr<TIPv6Addr> addr);
    SmartPtr<TIPv6Addr> getAddress();

    //DNS - oriented routines    
    void addDNSSrv(SmartPtr<TIPv6Addr> addr);
    void firstDNSSrv();
    SmartPtr<TIPv6Addr> getDNSSrv();
    void setDNSSrv(TContainer<SmartPtr<TIPv6Addr> > *dnsSrv);
    void setDomain(string domain);
    string getDomain();

    //NTP - oriented routines
    void addNTPSrv(SmartPtr<TIPv6Addr> addr);
    void firstNTPSrv();
    SmartPtr<TIPv6Addr> getNTPSrv();
    void setNTPSrv(TContainer<SmartPtr<TIPv6Addr> > *NTPSrv);
    void setTimeZone(string timeZone);
    string getTimeZone();

    void setClntMaxLease(long maxLeases);
    long getClntMaxLease();
    void setIfaceMaxLease(long maxLease);
    long getIfaceMaxLease();

    void setPreference(char pref);
    char getPreference();
    void setUnicast(bool unicast);
    bool getUnicast();
    void setRapidCommit(bool rapidComm);
    bool getRapidCommit();

private:
    bool UniAddress;
    char Preference;
    bool Unicast;
    bool RapidCommit;

    long IfaceMaxLease;
    long ClntMaxLease;
    SmartPtr<TIPv6Addr> Address;
    TContainer<SmartPtr<TIPv6Addr> > DNSSrv;		
    string Domain;			
    TContainer<SmartPtr<TIPv6Addr> > NTPSrv;		
    string TimeZone;		
};

#endif
