class TAddrClient;
#ifndef ADDRCLIENT_H
#define ADDRCLIENT_H

#include "SmartPtr.h"
#include "Container.h"
#include "AddrIA.h"
#include "DUID.h"

class TAddrClient
{
    friend ostream & operator<<(ostream & strum,TAddrClient &x);

public:
    TAddrClient(SmartPtr<TDUID> duid);
    SmartPtr<TDUID> getDUID();

    //--- IA list ---
    void firstIA();
    SmartPtr<TAddrIA> getIA();
    SmartPtr<TAddrIA> getIA(int IAID);
    void addIA(SmartPtr<TAddrIA> ia);
    bool delIA(long IAID);
    int countIA();

    // time related
    long getT1Timeout();
    long getT2Timeout();
    long getPrefTimeout();
    long getValidTimeout();

private:
    TContainer< SmartPtr<TAddrIA> > IAsLst;
    SmartPtr<TDUID> DUID;
};



#endif 
