/* 
 * File:   SrvGeolocMgr.cpp
 * Author: Michal Golon
 * 
 */

#include "SrvGeolocMgr.h"

#include "Opt.h"
#include "OptDUID.h"
#include "OptStringLst.h"
#include "SrvMsg.h"
#include "Portable.h"
#include "Logger.h"
#include <stdlib.h>

TSrvGeolocMgr * TSrvGeolocMgr::Instance = 0;

TSrvGeolocMgr::TSrvGeolocMgr(string xmlfile, string cfgfile) {
    // parse GeolocMgr config file
    if (!this->parseConfigFile(cfgfile)) {
        // if something is wrong, error and close app
        this->IsDone = true;
	return;
    }
    
    this->IsDone = false;
    
    // read from file and prepare cache
    this->cacheRead();
}

TSrvGeolocMgr::~TSrvGeolocMgr() {
    Log(Debug) << "SrvGeolocMgr cleanup." << LogEnd;
}

void TSrvGeolocMgr::instanceCreate( const std::string xmlFile, string cfgFile )
{
    if (Instance)
        Log(Crit) << "SrvAddrMgr already exists! Application error" << LogEnd;
    Instance = new TSrvGeolocMgr(xmlFile, cfgFile);
}

TSrvGeolocMgr & TSrvGeolocMgr::instance()
{
    if (!Instance)
        Log(Crit) << "SrvAddrMgr not created yet. Application error. Crashing in 3... 2... 1..." << LogEnd;
    return *Instance;
}

/*
 * Set maximum cache size -> count maximum number of geolocation entries.
 */
void TSrvGeolocMgr::setCacheSize() {      
    int entrySize = sizeof(TSrvGeolocEntry) + sizeof(TIPv6Addr) + sizeof(TDUID) + sizeof(List(string)) + 8*sizeof(string) + sizeof(time_t);
    this->MaxCacheSize = MAX_CACHE_SIZE/entrySize;
    
    Log(Debug) << "GeolocInfo cache: size set to " << MAX_CACHE_SIZE;
    Log(Cont) << " bytes, 1 cache entry size is " << entrySize;
    Log(Cont) << " bytes, so maximum " << this->MaxCacheSize;
    Log(Cont)<< " address-geoloc info pair(s) may be cached." << LogEnd;
    
    this->checkCacheSize();
}

/*
 * Check if able to add entry to cache.
 * If not (cache is full), remove from cache as many entries as needed.
 */
void TSrvGeolocMgr::checkCacheSize() {
    if (this->Geoloc.count() <= this->MaxCacheSize)
	return;

    // there are too many cached elements, delete some (first is the oldest)
    while (this->Geoloc.count() > this->MaxCacheSize) {
	this->Geoloc.delFirst();
    }
}

/*
 * Parse geolocation configuration file.
 * File contains validation time of geolocation entry.
 * 
 * If file doesn't exist, there should be an error and app should be closed.
 * 
 * If file exists, read two lines, from which second one contains validation time
 * in hours.
 */
bool TSrvGeolocMgr::parseConfigFile(string cfgFile) {
    ifstream f;
    string timeInHours;

    f.open(cfgFile.c_str());
    if (!f.is_open()) {
        Log(Crit) << "Unable to open " << cfgFile << " file." << LogEnd;
	return false;
    } else {
	Log(Notice) << "Parsing " << cfgFile << " config file..." << LogEnd;
    }
    
    getline(f, timeInHours); // junk
    getline(f, timeInHours);
    
    this->ValidationTime = atoi(timeInHours.c_str()) * 60 * 60;
    
    Log(Debug) << "Validation time of geolocation entry is: ";
    Log(Cont) << this->ValidationTime << " seconds." << LogEnd;
    
    Log(Notice) << "Parsing " << cfgFile << " done." << LogEnd;
    
    f.close();

    return true;
}

bool TSrvGeolocMgr::isDone() {
    return this->IsDone;
}

/*
 * Function for processing geolocation message.
 * 
 * Reads coordinates and other info from message and adds/updates geoloc entry.
 */
void TSrvGeolocMgr::process(SPtr<TSrvMsg> msg) {  
    // getting list of coordinates
    SPtr<TOpt> y = msg->getOption(OPTION_GEOLOC);
    SPtr<TOptStringLst> coordinates = (Ptr*) y;   
    List(string) list = coordinates->getList();
        
    // THIS RETURNS LINK-LOCAL!
    //SPtr<TIPv6Addr> addr = msg->getAddr();
         
    // getting client's DUID from message
    SPtr<TOpt> x = msg->getOption(OPTION_CLIENTID);
    SPtr<TOptDUID> optDuid = (Ptr*) x;
    SPtr<TDUID> duid = optDuid->getDUID();
    
    // getting global address from specific DUID (from address cache)
    SPtr<TIPv6Addr> addr = SrvAddrMgr().getCachedAddr(duid);
    
    // getting timestamp
    time_t timestamp = time(NULL);
    
    // check if entry already exists; if not - add new entry; if yes - update entry
    if(this->checkGeolocInfo(duid, addr)) {
        Log(Info) << "GeolocInfo cache: Updating geolocation information for DUID: ";
        Log(Cont) << duid->getPlain() << LogEnd;
        this->updateGeolocInfo(duid, addr, list, timestamp);
    } else {
        Log(Info) << "GeolocInfo cache: Adding geolocation information for DUID: ";
        Log(Cont) << duid->getPlain() << LogEnd;
        this->checkCacheSize();
        this->addGeolocInfo(duid, addr, list, timestamp);
    }
}

/*
 * Adding new entry to cache.
 */
void TSrvGeolocMgr::addGeolocInfo(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> cachedAddr, List(string) coordinates, time_t timestamp) {
    SPtr<TSrvGeolocEntry> entry;

    entry = new TSrvGeolocEntry();
    entry->Duid        = clntDuid;
    entry->Addr        = cachedAddr;
    entry->Coordinates = coordinates;
    entry->Timestamp   = timestamp;
    
    Log(Debug) << "GeolocInfo cache: Address " << cachedAddr->getPlain();
    Log(Cont) << " added for client (DUID=" << clntDuid->getPlain() << "). " << LogEnd; 
    
    this->Geoloc.append(entry);
}

/*
 * Updating entry from cache (de facto: first remove, then add as new).
 */
void TSrvGeolocMgr::updateGeolocInfo(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> cachedAddr, List(string) coordinates, time_t timestamp) {
    this->removeGeolocInfo(clntDuid, cachedAddr);
    this->addGeolocInfo(clntDuid, cachedAddr, coordinates, timestamp);
}

/*
 * Removing entry from cache.
 */
void TSrvGeolocMgr::removeGeolocInfo(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> cachedAddr) {
    SPtr<TSrvGeolocEntry> entry;

    this->Geoloc.first();
    while (entry = this->Geoloc.get()) {
        if((*entry->Duid) == *clntDuid && (*entry->Addr) == *cachedAddr)  {
            this->Geoloc.del();
            return;
        }
    }
}

/*
 * Check if specific entry exists in cache.
 */
bool TSrvGeolocMgr::checkGeolocInfo(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> cachedAddr) {
    SPtr<TSrvGeolocEntry> entry;

    this->Geoloc.first();
    while (entry = this->Geoloc.get()) {
        if((*entry->Duid) == *clntDuid && (*entry->Addr) == *cachedAddr) {
            return true;
        }
    }
    return false;
}

/*
 * Dump cache to file.
 */
void TSrvGeolocMgr::dump() {
    std::ofstream f;
    
    f.open(SRVGEOLOC_FILE, ios::trunc);
    if (!f.is_open()) {
	Log(Error) << "File " << SRVGEOLOC_FILE << " creation failed." << LogEnd;
	return;
    }
    
    SPtr<TSrvGeolocEntry> entry;
    SPtr<string> longitude;
    SPtr<string> latitude;
    const char * c;
    
    f << "<GeolocInfo>" << endl;
    f << "  <cache size=\"" << this->Geoloc.count() << "\">" << endl;

    this->Geoloc.first();
    while (entry = this->Geoloc.get()) {       
	f << "  <GeolocEntry>" <<endl;
        if(entry->Timestamp) {
            f << "      <timestamp>" << entry->Timestamp << "</timestamp>" << endl;
        }
        if(entry->Duid) {
            f << "      <duid>" << entry->Duid->getPlain() << "</duid>" << endl;
        }
        if(entry->Addr) {
            f << "      <addr>" << entry->Addr->getPlain() << "</addr>" << endl;
        }
        //if(entry->Coordinates.count()) {
            f << "      <coordinates>" << endl;
            entry->Coordinates.first();
            while (longitude = entry->Coordinates.get()) {
                f << "          <coordinate>" << endl;
                c = longitude->c_str();
                f << "                  <longitude>" << c << "</longitude>" << endl;
                latitude = entry->Coordinates.get();
                c = latitude->c_str();
                f << "                  <latitude>" << c << "</latitude>" << endl;
                f << "          </coordinate>" << endl;
            }            
            f << "      </coordinates>" << endl;
       // }
	f << "  </GeolocEntry>" << endl;
    }
    f << "</GeolocInfo>" << endl;
    
    f.close();
}

/*
 * Read cache from file.
 */
void TSrvGeolocMgr::cacheRead() {
    this->Geoloc.clear();
    
    ifstream f;
    f.open(SRVGEOLOC_FILE);
    if (!f.is_open()) {
	Log(Warning) << "GeolocInfo cache: Unable to open cache file ";
        Log(Cont) << SRVGEOLOC_FILE << "." << LogEnd;
	return;
    }
    
    bool started = false;
    bool ended = false;
    bool parsed = false;
    bool entryStarted = false;
    bool entryEnded = false;
    
    int lineno = 0;
    int entries = 0;
    int entriesOmitted = 0;
    
    time_t timestamp = time(NULL);
    
    string s;
    
    while (!f.eof()) {
	parsed = false;
	string::size_type pos=0;
        entryStarted = false;
        entryEnded = false;
        long int entryTimestamp = 0;
        
        getline(f, s);
        
	if ((pos = s.find("<cache")) != string::npos) {
	    started = true;
	    if ((pos = s.find("size=\"")) != string::npos) {
		s = s.substr(pos+6);
		entries = atoi(s.c_str());
		Log(Debug) << "GeolocInfo cache: " << SRVGEOLOC_FILE;
                Log(Cont) << " file: parsing started, expecting ";
                Log(Cont) << entries << " entries." << LogEnd;
	    } else {
		Log(Debug) << "GeolocInfo cache: " << SRVGEOLOC_FILE;
                Log(Cont) << " file:unable to find entries count. size=\"...\" missing in line ";
		Log(Cont) << lineno << "." << LogEnd;
		return;
	    }
	}
	
	if (s.find("<GeolocEntry") != string::npos) {
	    if (!started) {
		Log(Error) << "GeolocInfo cache: " << SRVGEOLOC_FILE;
                Log(Cont) << " file: opening tag <cache> missing." << LogEnd;
		return;
	    }
            
            entryStarted = true;
            List(string) entryCoordinates;
            string entryDuid;
            string entryAddr;
            lineno = 0;
            
            while(entryEnded == false) {
                getline(f, s);
                pos=0;

                if ((pos = s.find("<timestamp>")) != string::npos) {
                    s = s.substr(pos+11);
                    string timestampString = s.substr(0, s.find("<"));
                    entryTimestamp = atoi(timestampString.c_str());
                    lineno++;
                    
                    // if geolocation entry has expired, we should omit it
                    if((timestamp - entryTimestamp) > this->ValidationTime) {
                        entriesOmitted++;
                        // geolocation entry should have 23 lines
                        // (minus <GeolocEntry> and <timestamp>) -> so 21 lines
                        for(int i = 0; i < 21; i++) {
                            getline(f, s);
                        }
                        
                        if (s.find("</GeolocEntry>") == string::npos) {
                            Log(Error) << "GeolocInfo cache: " << SRVGEOLOC_FILE;
                            Log(Cont) << " file: insufficient data in geolocation entry." << LogEnd;
                            return;
                        }
                        
                        entryEnded = true;
                        continue;
                    }
                }
                
                if ((pos = s.find("<duid>")) != string::npos) {
                    s = s.substr(pos+6);
                    entryDuid = s.substr(0, s.find("<"));
                    lineno++;
                }
                
                if ((pos = s.find("<addr>")) != string::npos) {
                    s = s.substr(pos+6);
                    entryAddr = s.substr(0, s.find("<"));
                    lineno++;
                }
                
                if ((pos = s.find("<longitude>")) != string::npos) {
                    s = s.substr(pos+11);
                    SPtr<string> longitude = new string(s.substr(0, s.find("<")));
                    entryCoordinates.append(longitude);
                    lineno++;
                }
                
                if ((pos = s.find("<latitude>")) != string::npos) {
                    s = s.substr(pos+10);
                    SPtr<string> latitude = new string(s.substr(0, s.find("<")));
                    entryCoordinates.append(latitude);
                    lineno++;
                }
                
                if(s.find("coordinate") != string::npos) {
                    lineno++;
                }
                
                if (s.find("</GeolocEntry>") != string::npos) {
                    // geolocation entry should have 23 lines
                    // (minus <GeolocEntry> and <timestamp>) -> so 21 lines
                    if(lineno == 21) {
                        // add entry from file to cache
                        SPtr<TIPv6Addr> addr = new TIPv6Addr(entryAddr.c_str(), true);
                        SPtr<TDUID>     duid = new TDUID(entryDuid.c_str());
                        this->addGeolocInfo(duid, addr, entryCoordinates, entryTimestamp);
                        entryEnded = true;
                    } else {
                        Log(Error) << "GeolocInfo cache: " << SRVGEOLOC_FILE;
                        Log(Cont) << " file: insufficient data in geolocation entry."; 
                        Log(Cont) << " No entries cached!" << LogEnd;
                        return;
                    }
                }
            }    
	}
        
        if (s.find("</GeolocInfo") != string::npos) {
	    ended = true;
        }
    }
    
    if(entriesOmitted) {
        Log(Debug) << "GeolocInfo cache: " << SRVGEOLOC_FILE;
        Log(Cont) << " file: " << entriesOmitted << " entries omitted." << LogEnd;
    }
    
    if(this->Geoloc.count() != entries) {
	Log(Debug) << "GeolocInfo cache: " << SRVGEOLOC_FILE;
        Log(Cont) << " file: " << entries << " entries expected, but ";
        Log(Cont) << this->Geoloc.count() << " cached." << LogEnd;
    }
    
    if(!ended) {
        Log(Debug) << "GeolocInfo cache: " << SRVGEOLOC_FILE;
        Log(Cont) << " file: missing closing tag!" << LogEnd;
    }
    
    f.close();
}

/*
 * Get coordinates for specific DUID from cache.
 */
List(string) TSrvGeolocMgr::getGeolocInfo(SPtr<TDUID> duid) {
    SPtr<TSrvGeolocEntry> entry;
    List(string) emptyList;
    
    this->Geoloc.first();
    while (entry = this->Geoloc.get()) {
        if(*entry->Duid == *duid) {
            return entry->Coordinates;
        }
    }
    
    return emptyList;
}

/*
 * Check if there are some entries that has been outdated.
 * 
 * If yes, delete them and them dump cache to file.
 */
void TSrvGeolocMgr::doDuties() {   
    SPtr<TSrvGeolocEntry> entry;
    bool anyDeleted = false;
    time_t timestamp = time(NULL); 
    
    this->Geoloc.first();
    while(entry = this->Geoloc.get()) {
        if((timestamp - entry->Timestamp) > this->ValidationTime) {
            this->Geoloc.del();
            anyDeleted = true;
            Log(Notice) << "Geoloc entry for duid " << entry->Duid << " (addr: ";
            Log(Cont) << entry->Addr << "\") has expired." << LogEnd;
        }
    }
    
    if(anyDeleted) {
        this->dump();  
    }
}