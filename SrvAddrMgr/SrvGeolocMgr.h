/* 
 * File:   SrvGeolocMgr.h
 * Author: root
 *
 * Created on 12 wrzesień 2011, 20:10
 */

#ifndef SRVGEOLOCMGR_H
#define	SRVGEOLOCMGR_H

#include "AddrMgr.h"
#include "SrvMsg.h"

#define SrvGeolocMgr() (TSrvGeolocMgr::instance())

class TSrvGeolocMgr {
public:
    static void instanceCreate(const std::string xmlFile, string cfgfile);
    static TSrvGeolocMgr & instance();

    class TSrvGeolocEntry
    {
    public:
      SPtr<TIPv6Addr> Addr;             // cached address, previously assigned to a client
      SPtr<TDUID>     Duid;             // client's duid    
      List(string)    Coordinates;      // coordinates
      time_t          Timestamp;        // timestamp
    };

    ~TSrvGeolocMgr();
    
    void addGeolocInfo(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> cachedAddr, List(string) coordinates, time_t timestamp);
    void updateGeolocInfo(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> cachedAddr, List(string) coordinates, time_t timestamp);
    void removeGeolocInfo(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> cachedAddr);
    bool checkGeolocInfo(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> cachedAddr);
    void process(SPtr<TSrvMsg> msg);
    void dump();
    void cacheRead();
    void setCacheSize();
    void checkCacheSize();
    bool isDone();
    List(string) getGeolocInfo(SPtr<TDUID> duid);
    void doDuties();
        
 private:
    TSrvGeolocMgr(string xmlfile, string cfgfile);
    bool parseConfigFile(string cfgfile);
    static TSrvGeolocMgr * Instance;
    List(TSrvGeolocEntry) Geoloc;       // list of cached addresses and geoloc infos
    int MaxCacheSize;                   // maximum numbers of entries
    long int ValidationTime;            // validation time of entry
    bool IsDone;
};

#endif	/* SRVGEOLOCMGR_H */

