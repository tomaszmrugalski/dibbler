#ifndef TSRVPARSCLASS_H_
#define TSRVPARSCLASS_H_
#include <string>
#include "StationID.h"
#include "DHCPConst.h"
#include "Container.h"
#include "SmartPtr.h"
#include "StationRange.h"
#include "IPv6Addr.h"

class TSrvParsClassOpt
{
public:
	TSrvParsClassOpt(void);
	~TSrvParsClassOpt(void);

    //T1,T2,Valid,Prefered time routines
    void setT1Beg(long t1);
    void setT1End(long t1);
    long getT1Beg();
    long getT1End();

    void setT2Beg(long t2);
    void setT2End(long t2);
    long getT2Beg();
    long getT2End();
    
    void setPrefBeg(long pref);
    void setPrefEnd(long pref);
    long getPrefBeg();
    long getPrefEnd();

    void setValidEnd(long valid);
    void setValidBeg(long valid);
    long getValidEnd();
    long getValidBeg();
        
    //Rejected clients access routines
    void addRejedClnt(SmartPtr<TStationRange> addr);
    void firstRejedClnt();
    SmartPtr<TStationRange> getRejedClnt();
    void setRejedClnt(TContainer<SmartPtr<TStationRange> > *rejedClnt);
    
    //Accepted clients access routines
    void addAcceptClnt(SmartPtr<TStationRange> addr);
    void firstAcceptClnt();
    SmartPtr<TStationRange> getAcceptClnt();
    void setAcceptClnt(TContainer<SmartPtr<TStationRange> > *acceptClnt);
    
    //Pool access routines
    void addPool(SmartPtr<TStationRange> addr);
    void firstPool();
    SmartPtr<TStationRange> getPool();
    void setPool(TContainer<SmartPtr<TStationRange> > *pool);
    long countPool();

    void setPreference(char pref);
    char getPreference();
    void setUnicast(bool unicast);
    bool getUnicast();
    void setRapidCommit(bool rapidComm);
	bool getRapidCommit();
    void setMaxLeases(long maxLeases);
	long getMaxLeases();
    void setMaxClientLease(long maxClntLeases);
	long getMaxClientLease();

private:
    //Ranges of T1 i T2
	long T1Beg;
	long T1End;
    long T2End;
    long T2Beg;
	long PrefBeg;
	long PrefEnd;
	long ValidBeg;
	long ValidEnd;
	
    TContainer<SmartPtr<TStationRange> > RejedClnt;
	TContainer<SmartPtr<TStationRange> > AcceptClnt;
	TContainer<SmartPtr<TStationRange> > Pool;
	
    char                Preference;
	bool				Unicast;
	bool				RapidCommit;
    long                MaxLeases;
	long                MaxClientLeases;
};
#endif
