#ifndef CLNTADDRMGR_H_HEADER_INCLUDED_C109774C
#define CLNTADDRMGR_H_HEADER_INCLUDED_C109774C

#include "Container.h"
#include "SmartPtr.h"
#include "AddrIA.h"
#include "AddrMgr.h"
#include "ClntCfgMgr.h"

class TClntCfgMgr;

class TClntAddrMgr : public TAddrMgr
{
  public:
    //## Wczytuje baz� danych z dysku. Sprawdza r�wnie�, czy wszystkie
    //## interfejsy s� dost�pne i nale�y je konfigurowa� komunikuj�c si� z
    //## ConfMgr'em
    TClntAddrMgr(SmartPtr<TClntCfgMgr> ClntConfMgr, string addrdb, bool loadDB);

    long getT1Timeout();
    long getT2Timeout();
    long getPrefTimeout();
    long getValidTimeout();

    unsigned long getTimeout();
    unsigned long getTentativeTimeout();
    void firstIA();
    SmartPtr<TAddrIA> getIA();
    SmartPtr<TAddrIA> getIA(long IAID);
    void addIA(SmartPtr<TAddrIA> ptr);
    bool delIA(long IAID);
    int countIA();
    ~TClntAddrMgr();

    void doDuties();
    
    bool isIAAssigned(long IAID);
 private:
    SmartPtr<TAddrClient> Client;
};



#endif
