/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: AddrClient.h,v 1.9 2008-02-25 17:49:06 thomson Exp $
 *
 */

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
    SmartPtr<TAddrIA> getIA(unsigned long IAID);
    void addIA(SmartPtr<TAddrIA> ia);
    bool delIA(unsigned long IAID);
    int countIA();

    //--- PD list ---
    void firstPD();
    SmartPtr<TAddrIA> getPD();
    SmartPtr<TAddrIA> getPD(unsigned long IAID);
    void addPD(SmartPtr<TAddrIA> ia);
    bool delPD(unsigned long IAID);
    int countPD();

    //--- TA list ---
    void firstTA();
    SmartPtr<TAddrIA> getTA();
    SmartPtr<TAddrIA> getTA(unsigned long iaid);
    void addTA(SmartPtr<TAddrIA> ia);
    bool delTA(unsigned long iaid);
    int countTA();

    // time related
    unsigned long getT1Timeout();
    unsigned long getT2Timeout();
    unsigned long getPrefTimeout();
    unsigned long getValidTimeout();

    //authentication
    uint32_t getSPI();
    void setSPI(uint32_t val);
    uint64_t getReplayDetectionRcvd();
    void setReplayDetectionRcvd(uint64_t val);
    uint64_t getNextReplayDetectionSent();

    unsigned long getLastTimestamp();
    
private:
    List(TAddrIA) IAsLst;
    List(TAddrIA) TALst;
    List(TAddrIA) PDLst;
    SmartPtr<TDUID> DUID;

    uint32_t SPI;
    uint64_t ReplayDetectionRcvd;
    uint64_t ReplayDetectionSent;
};



#endif 
