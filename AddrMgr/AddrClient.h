/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 licence
 *
 */

class TAddrClient;
#ifndef ADDRCLIENT_H
#define ADDRCLIENT_H

#include <vector>
#include "SmartPtr.h"
#include "Container.h"
#include "AddrIA.h"
#include "DUID.h"
#include "Portable.h"

class TAddrClient
{
    friend std::ostream & operator<<(std::ostream & strum, TAddrClient &x);

public:
    TAddrClient(SPtr<TDUID> duid);
    SPtr<TDUID> getDUID();

    //--- IA list ---
    void firstIA();
    SPtr<TAddrIA> getIA();
    SPtr<TAddrIA> getIA(unsigned long IAID);
    void addIA(SPtr<TAddrIA> ia);
    bool delIA(unsigned long IAID);
    int countIA();

    //--- PD list ---
    void firstPD();
    SPtr<TAddrIA> getPD();
    SPtr<TAddrIA> getPD(unsigned long IAID);
    void addPD(SPtr<TAddrIA> ia);
    bool delPD(unsigned long IAID);
    int countPD();

    //--- TA list ---
    void firstTA();
    SPtr<TAddrIA> getTA();
    SPtr<TAddrIA> getTA(unsigned long iaid);
    void addTA(SPtr<TAddrIA> ia);
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

    void generateReconfKey();

    unsigned long getLastTimestamp();

    /// @brief 128 bits of pure randomness used in reconfigure process
    ///
    /// Reconfigure Key nonce is set be the server and the stored by the client.
    /// It is later used to check if the reconfigure message came from the same
    /// server that initially provided the configuration.
    std::vector<uint8_t> ReconfKey_;

private:
    List(TAddrIA) IAsLst;
    List(TAddrIA) TALst;
    List(TAddrIA) PDLst;
    SPtr<TDUID> DUID_;

    uint32_t SPI_;
    uint64_t ReplayDetectionRcvd_;
};



#endif
