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


private:
	bool			    UniAddress;
	SmartPtr<TIPv6Addr> Address;
    TContainer<SmartPtr<TIPv6Addr> > DNSSrv;		
	string				Domain;			
	TContainer<SmartPtr<TIPv6Addr> > NTPSrv;		
	string				TimeZone;		
	//string				NISServer;		
};

#endif