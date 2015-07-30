/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <net/if.h>
#endif
#include "Iface.h"
#include "Portable.h"
#include "Logger.h"



using namespace std;
/*
 * stores informations about interface
 */
TIfaceIface::TIfaceIface(const char * name, int id, unsigned int flags, char* mac,
                         int maclen, char* llAddr, int llAddrCnt, char * globalAddr,
                         int globalCnt, int hwType)
    :Mac(new char[maclen]),Maclen(maclen), M_bit_(false), O_bit_(false)
{
    snprintf(this->Name,MAX_IFNAME_LENGTH,"%s",name);

    this->ID = id;
    this->Flags = flags;

    // store mac address
    memcpy(this->Mac,mac,maclen);

    // store all link-layer addresses
    this->LLAddrCnt=llAddrCnt;
    if (llAddrCnt>0)
    {
        this->LLAddr=new char[16*llAddrCnt];
        memcpy(this->LLAddr,llAddr,16*llAddrCnt);
    }
    else
        this->LLAddr=NULL;
    this->PresLLAddr=this->LLAddr;

    // store all global addresses
    for (int i=0; i<globalCnt; i++) {
        SPtr<TIPv6Addr> addr = new TIPv6Addr(globalAddr+16*i);
        this->GlobalAddrLst.append(addr);
    }

    // store hardware type
    this->HWType=hwType;
}

/*
 * returns interface name
 */
char* TIfaceIface::getName() {
    return this->Name;
}

/*
 * returns interface ID
 */
int TIfaceIface::getID() {
    return this->ID;
}

string TIfaceIface::getFullName() {
    ostringstream oss;
    oss << this->ID;
    return string(this->Name)
        +"/"
        +oss.str();
}

/*
 * return interface flags
 */
unsigned int TIfaceIface::getFlags() {
    return this->Flags;
}

void TIfaceIface::updateState(struct iface *x)
{
    this->Flags = x->flags;
    delete [] Mac;
    Mac = new char[Maclen];
    Maclen = x->maclen;
    memcpy(Mac, x->mac, Maclen);

    if (LLAddrCnt && LLAddr) {
        delete [] LLAddr;
        LLAddrCnt = 0;
        LLAddr = 0;
    }
    LLAddrCnt = x->linkaddrcount;
    if (LLAddrCnt) {
        LLAddr = new char[16*LLAddrCnt];
        memcpy(LLAddr, x->linkaddr, 16*LLAddrCnt);
    }
    PresLLAddr = LLAddr;

    GlobalAddrLst.clear();

    // store all global addresses
    for (int i=0; i<x->globaladdrcount; i++) {
        SPtr<TIPv6Addr> addr = new TIPv6Addr(x->globaladdr+16*i);
        this->GlobalAddrLst.append(addr);
    }

    HWType = x->hardwareType;

    setMBit(x->m_bit);
    setOBit(x->o_bit);
}


/**
 * returns true if interface is UP
 */
bool TIfaceIface::flagUp()
{
    return this->Flags & IFF_UP;
}

/**
 * returns true if interface is RUNNING
 */
bool TIfaceIface::flagRunning()
{
    return (bool)(this->Flags & IFF_RUNNING);
}

/**
 * returns true is interface is MULTICAST capable
 */
bool TIfaceIface::flagMulticast()
{
    return (Flags&IFF_MULTICAST)?true:false;
}

/**
 * returns true is interface is LOOPBACK
 */
bool TIfaceIface::flagLoopback()
{
    return (Flags&IFF_LOOPBACK)?true:false;
}

/**
 * returns MAC length
 */
int TIfaceIface::getMacLen()
{
    return Maclen;
}

/**
 * returns MAC
 */
char* TIfaceIface::getMac()
{
    return Mac;
}

void TIfaceIface::firstGlobalAddr()
{
    GlobalAddrLst.first();
}

SPtr<TIPv6Addr> TIfaceIface::getGlobalAddr() {
    return this->GlobalAddrLst.get();
}

unsigned int TIfaceIface::countGlobalAddr() {
    return this->GlobalAddrLst.count();
}

void TIfaceIface::addGlobalAddr(SPtr<TIPv6Addr> addr)
{
    this->GlobalAddrLst.append(addr);
}

void TIfaceIface::delGlobalAddr(SPtr<TIPv6Addr> addr)
{
    SPtr<TIPv6Addr> tempAddr;
    for(unsigned int i = 0; i < this->GlobalAddrLst.count(); i++)
    {
        tempAddr = this->GlobalAddrLst.get();
        if(*tempAddr == *addr){
            this->GlobalAddrLst.del();
            break;
        }
    }
}


/**
 * returns HW type
 */
int TIfaceIface::getHardwareType() {
    return this->HWType;
}

// --------------------------------------------------------------------
// --- address related ------------------------------------------------
// --------------------------------------------------------------------

/**
 * adds address to this interface with prefered- and valid-lifetime
 * (wrapper around pure C function)
 */
bool TIfaceIface::addAddr(SPtr<TIPv6Addr> addr,long pref, long valid, int prefixLen) {
    Log(Notice) << "Address " << addr->getPlain() << "/" << prefixLen << " added to "
                << getFullName() << " interface." << LogEnd;

    return (bool)ipaddr_add(this->Name, this->ID, addr->getPlain(),
                            pref, valid, prefixLen);
}

/**
 * deletes address from interface
 * (wrapper around pure C function)
 */
bool TIfaceIface::delAddr(SPtr<TIPv6Addr> addr, int prefixLen) {
    Log(Notice) << "Address " << addr->getPlain() << "/" << prefixLen << " deleted from "
                << getFullName() << " interface." << LogEnd;
    return (bool)ipaddr_del( this->Name, this->ID, addr->getPlain(), prefixLen);
}

/**
 * update address prefered- and valid-lifetime
 */
bool TIfaceIface::updateAddr(SPtr<TIPv6Addr> addr, long pref, long valid) {
    int result;
    Log(Notice) << "Address " << addr->getPlain() << " updated on "
                << getFullName() << " interface." << LogEnd;

    result = ipaddr_update((char *)this->Name, this->ID, (char *)addr->getPlain(),
                           pref, valid, this->PrefixLen);

    if (result!=LOWLEVEL_NO_ERROR)
        return false;

    return true;
}

/**
 * get first link-local address
 */
char* TIfaceIface::firstLLAddress() {
    return PresLLAddr=LLAddr;
}

/**
 * get next link-local address
 * (oh boy, this method stinks. Nobody uses it, anyway)
 */
char* TIfaceIface::getLLAddress() {
    char* retVal;
    if( (retVal=this->PresLLAddr) ) {
        if ( (this->PresLLAddr-this->LLAddr) < (16*this->LLAddrCnt) )
            this->PresLLAddr+=16;
        else
            this->PresLLAddr=NULL;
    }
    return retVal;
}

int TIfaceIface::countLLAddress() {
    return this->LLAddrCnt;
}

// --------------------------------------------------------------------
// --- socket related -------------------------------------------------
// --------------------------------------------------------------------

/*
 * binds socket to one address only
 */
bool TIfaceIface::addSocket(SPtr<TIPv6Addr> addr,int port, bool ifaceonly, bool reuse) {
    // Log(Debug) << "Creating socket on " << *addr << " address." << LogEnd;
    SPtr<TIfaceSocket> ptr = new TIfaceSocket(this->Name, this->ID, port, addr, ifaceonly, reuse);
    if (ptr->getStatus()!=STATE_CONFIGURED) {
        return false;
    }
    SocketsLst.append(ptr);
    return true;
}

#if 0
/*
 * binds socket on whole interface
 */
bool TIfaceIface::addSocket(int port, bool ifaceonly, bool reuse) {
    SPtr<TIfaceSocket> ptr =
        new TIfaceSocket(this->Name, this->ID, port, ifaceonly, reuse);
    if (ptr->getStatus()!=STATE_CONFIGURED) {
              return false;
    }
    SocketsLst.append(ptr);
    return true;
}
#endif

/*
 * closes socket
 */
bool TIfaceIface::delSocket(int fd) {
    SPtr<TIfaceSocket> sock;
    SocketsLst.first();

    while ( sock = SocketsLst.get() ) {
              if (sock->getFD() == fd) {
                  SocketsLst.del();
                  return true;
              }
    }
    return false;
}

/*
 * rewinds sockets list to the beginning
 */
void TIfaceIface::firstSocket() {
    SocketsLst.first();
}

/*
 * returns next socket from list
 */
SPtr <TIfaceSocket> TIfaceIface::getSocket() {
    return SocketsLst.get();
}

/*
 * returns socket by FileDescriptor (or NULL, if no such socket exists)
 */
SPtr<TIfaceSocket> TIfaceIface::getSocketByFD(int fd) {
    SPtr<TIfaceSocket> ptr;
    SocketsLst.first();
    while ( ptr = SocketsLst.get() ) {
              if ( ptr->getFD()==fd )
                  return ptr;
    }
    return SPtr<TIfaceSocket>(); // NULL
}

/*
 * returns sockets count
 */
int TIfaceIface::countSocket() {
    return SocketsLst.count();
}

/*
 *  releases data allocated for ll addresses
 */
TIfaceIface::~TIfaceIface() {
    if (this->LLAddrCnt>0) {
        delete [] this->LLAddr;
    }
    delete [] Mac;
}

SPtr<TIfaceSocket> TIfaceIface::getSocketByAddr(SPtr<TIPv6Addr> addr) {
    SPtr<TIfaceSocket> ptr;
    SocketsLst.first();
    while ( ptr = SocketsLst.get() ) {
              if ( *ptr->getAddr()==*addr )
                  return ptr;
    }
    return SPtr<TIfaceSocket>(); // NULL
}

void TIfaceIface::setPrefixLength(int len) {
    if (len>128 || len<0) {
              Log(Error) << "Invalid length " << len << " set attempt was ignored on the " << this->getFullName() << " interface." << LogEnd;
              return;
    }
    this->PrefixLen = len;
}

int TIfaceIface::getPrefixLength() {
    return PrefixLen;
}

/// @brief set M (managed) bit as received from Router Advertisement
///
/// @param m bool flag (Sets of clears M bit)
void TIfaceIface::setMBit(bool m) {
    M_bit_ = m;
}

/// @brief set O (other conf) bit as received from Router Advertisement
///
/// @param o bool flag (Sets of clears O bit)
void TIfaceIface::setOBit(bool o) {
    O_bit_ = o;
}

/// @brief returns M (managed) bit as received from Router Advertisement
///
/// @return value of M bit
bool TIfaceIface::getMBit() {
    return M_bit_;
}

/// @brief returns O (other conf) bit as received from Router Advertisement
///
/// @return value of O bit
bool TIfaceIface::getOBit() {
    return O_bit_;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------
/*
 * just prints important informations (debugging & logging)
 */
ostream & operator <<(ostream & strum, TIfaceIface &x) {
    char buf[48];

    strum << "  <IfaceIface"
          << " name=\"" << x.Name << "\""
          << " ifindex=\"" << x.ID << "\""
          << " hwType=\"" << x.getHardwareType() << "\""
          << " flags=\"0x" << hex << x.Flags << dec << "\" "
          << " mBit=\"" << (x.M_bit_?"1":"0") << "\" oBit=\"" << (x.O_bit_?"1":"0")
          << "\">" << endl
          << "    <!-- " << (x.flagLoopback()?"looback":"no-loopback")
          << (x.flagRunning()?" running":" no-running")
          << (x.flagMulticast()?" multicast -->":" no-multicast -->") << endl
          << "    <!-- PrefixLength configured to " << x.PrefixLen << " -->" << endl
          << "    <!-- " << x.LLAddrCnt << " link scoped addrs -->" << endl;

    for (int i=0; i<x.LLAddrCnt; i++) {
              inet_ntop6(x.LLAddr+i*16,buf);
              strum << "    <Addr>" << buf << "</Addr>" << endl;
    }

    strum << "    <!-- " << x.countGlobalAddr() << " non-local addrs -->" << endl;

    x.firstGlobalAddr();
    SPtr<TIPv6Addr> addr;
    while (addr = x.getGlobalAddr()) {
        strum << "    " << *addr;
    }

    strum << "    <Mac>";
    for (int i=0; i<x.Maclen; i++) {
              strum.fill('0');
              strum.width(2);
              strum << (hex) << (int) x.Mac[i];
              if (i<x.Maclen-1) strum  << ":";
    }
    strum << "</Mac>" << endl;

    SPtr<TIfaceSocket> sock;
    x.firstSocket();
    while (sock = x.getSocket() ) {
        strum << "    " << *sock;
    }
    strum << "  </IfaceIface>" << endl;
    return strum;
}
