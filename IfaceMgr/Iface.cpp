#include <iostream>
#include "Iface.h"
#include "Portable.h"
#include "Logger.h"

/*
 * stores informations about interface
 */
TIfaceIface::TIfaceIface(char * name, int id, unsigned int flags, char* mac, 
			 int maclen, char* llAddr, int llAddrCnt, int hwType)
{
    snprintf(this->Name,MAX_IFNAME_LENGTH,"%s",name);
    this->ID = id;
    this->Flags = flags;

    // store mac address
    this->Maclen=maclen;
    this->Mac=new char[maclen];
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

/*
 * return interface flags
 */
unsigned int TIfaceIface::getFlags() {
    return this->Flags;
}

/*
 * returns true if interface is UP
 */
bool TIfaceIface::flagUp() {
    return this->Flags&IF_UP;
}

/*
 * returns true if interface is RUNNING
 */
bool TIfaceIface::flagRunning() {
    return this->Flags & IF_RUNNING;
}

/*
 * returns true is interface is MULTICAST capable
 */
bool TIfaceIface::flagMulticast() {
    return Flags&IF_MULTICAST;
}

/*
 * returns true is interface is LOOPBACK
 */
bool TIfaceIface::flagLoopback() {
    return Flags&IF_LOOPBACK;
}

/*
 * returns MAC length
 */
int TIfaceIface::getMacLen() {
    return this->Maclen;
}

/*
 * returns MAC
 */
char* TIfaceIface::getMac() {
    return this->Mac;
}

/*
 * returns HW type 
 */
int TIfaceIface::getHardwareType() {
    return this->HWType;
}

// --------------------------------------------------------------------
// --- address related ------------------------------------------------
// --------------------------------------------------------------------

/*
 * adds address to this interface with prefered- and valid-lifetime
 * (wrapper around pure C function)
 */
bool TIfaceIface::addAddr(SmartPtr<TIPv6Addr> addr,long pref, long valid) {
    return ipaddr_add( this->Name, this->ID, 
                      addr->getPlain(), pref, valid);
}

/*
 * deletes address from interface
 * (wrapper around pure C function)
 */
bool TIfaceIface::delAddr(SmartPtr<TIPv6Addr> addr) {
    return ipaddr_del( this->Name, this->ID, addr->getPlain());
}

/*
 * update address prefered- and valid-lifetime
 */
bool TIfaceIface::updateAddr(SmartPtr<TIPv6Addr> addr, long pref, long valid) {
#ifdef WIN32
    return ipaddr_add((char *)this->Name, this->ID, 
		      (char *)addr->getPlain(), pref, valid);
#endif

#ifdef LINUX
  // FIXME: Linux kernel currently does not provide API for dynamic adresses
#endif

}

/*
 * get first LL address
 */
char* TIfaceIface::firstLLAddress() {
    return PresLLAddr=LLAddr;
}

/*
 * get next LL address
 * (oh boy, this method stinks. Nobody uses it, anyway)
 */
char* TIfaceIface::getLLAddress() {
    char* retVal;
    if(retVal=this->PresLLAddr) {
        if ( (this->PresLLAddr-this->LLAddr) < (16*this->LLAddrCnt) )
            this->PresLLAddr+=16;
        else
            this->PresLLAddr=NULL;
    }
    return retVal;
}

// --------------------------------------------------------------------
// --- socket related -------------------------------------------------
// --------------------------------------------------------------------

/*
 * binds socket to one address only
 */
bool TIfaceIface::addSocket(SmartPtr<TIPv6Addr> addr,int port, bool ifaceonly) {
    SmartPtr<TIfaceSocketIPv6> ptr = 
	new TIfaceSocketIPv6(this->Name, this->ID, port, addr, ifaceonly);
    SocketsLst.append(ptr);
    return true;
}

/*
 * binds socket on whole interface
 */
bool TIfaceIface::addSocket(int port, bool ifaceonly) {
    SmartPtr<TIfaceSocketIPv6> ptr = 
	new TIfaceSocketIPv6(this->Name, this->ID, port, ifaceonly);
    SocketsLst.append(ptr);
    return true;
}

/*
 * closes socket
 */
bool TIfaceIface::delSocket(int fd) {
    SmartPtr<TIfaceSocketIPv6> sock;
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
SmartPtr <TIfaceSocketIPv6> TIfaceIface::getSocket() {
    return SocketsLst.get();
}

/*
 * returns socket by FileDescriptor (or NULL, if no such socket exists)
 */
SmartPtr <TIfaceSocketIPv6> TIfaceIface::getSocketByFD(int fd) {
    SmartPtr<TIfaceSocketIPv6> ptr;
    SocketsLst.first();
    while ( ptr = SocketsLst.get() ) {
	if ( ptr->getFD()==fd )
	    return ptr;
    }
    return 0; // NULL
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
	delete this->LLAddr;
    }
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------
/*
 * just prints important informations (debugging & logging)
 */
ostream & operator <<(ostream & strum, TIfaceIface &x) {
    char buf[48];

    strum << "  <TIfaceIface";
    strum << " name=\"" << x.Name << "\"";
    strum << " id=\"" << x.ID << "\"";
    strum << " flags=\"" << x.Flags << "\">" << endl;
    strum << "    <!-- " << x.LLAddrCnt << " link scoped addrs -->" << endl;

    for (int i=0; i<x.LLAddrCnt; i++) {
	inet_ntop6(x.LLAddr+i*16,buf);
	strum << "    <Addr>" << buf << "</Addr>" << endl;
    }

    strum << "    <Mac>";
    for (int i=0; i<x.Maclen; i++) {
	strum.fill('0');
	strum.width(2);
	strum << (hex) << (int) x.Mac[i];
	if (i<x.Maclen-1) strum  << ":";
    }
    strum << "</Mac>" << endl;

    SmartPtr<TIfaceSocketIPv6> sock;
    x.firstSocket();
    while (sock = x.getSocket() ) {
	strum << *sock;
    }
    strum << "  </TIfaceIface>" << endl;
    return strum;
}
