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
    //## Wczytuje bazê danych z dysku. Sprawdza równie¿, czy wszystkie
    //## interfejsy s± dostêpne i nale¿y je konfigurowaæ komunikuj±c siê z
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
