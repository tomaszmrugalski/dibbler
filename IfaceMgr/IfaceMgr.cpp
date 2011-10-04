/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: IfaceMgr.cpp,v 1.28 2008-08-29 00:07:30 thomson Exp $
 *
 */

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <errno.h>
#include "Portable.h"
#include "IfaceMgr.h"
#include "Iface.h"
#include "SocketIPv6.h"
#include "Logger.h"
#include "Msg.h"
#include "OptIAAddress.h"
#include "OptIAPrefix.h"

/*
 * creates list of interfaces
 */
TIfaceMgr::TIfaceMgr(string xmlFile, bool getIfaces)
{
    this->XmlFile = xmlFile;
    this->IsDone  = false;
    struct iface  * ptr;
    struct iface  * ifaceList;

    if (!getIfaces)
	return;

    // get interface list
    ifaceList = if_list_get(); // external (C coded) function
    ptr = ifaceList;
    
    if  (!ifaceList) {
	IsDone = true;
	Log(Crit) << "Unable to read info interfaces. Make sure "
		  << "you are using proper port (i.e. win32 on WindowsXP or 2003)"
		  << " and you have IPv6 support enabled." << LogEnd;
	return;
    }
    
    while (ptr!=NULL) {
        Log(Notice) << "Detected iface " << ptr->name << "/" << ptr->id 
                 // << ", flags=" << ptr->flags 
                    << ", MAC=" << this->printMac(ptr->mac, ptr->maclen) << "." << LogEnd;
	
        SPtr<TIfaceIface> iface(new TIfaceIface(ptr->name,ptr->id,
						       ptr->flags,
						       ptr->mac,
						       ptr->maclen,
						       ptr->linkaddr,
						       ptr->linkaddrcount,
						       ptr->globaladdr,
						       ptr->globaladdrcount,
						       ptr->hardwareType));
        this->IfaceLst.append(iface);
        ptr = ptr->next;
    }
    if_list_release(ifaceList); // allocated in pure C, and so release it there
}

/*
 * returns true if IfaceMgr has finished it's job (failed to get info about interfaces)
 */
bool TIfaceMgr::isDone() {
    return IsDone;
}

/*
 * rewinds interface list to first interface
 */
void TIfaceMgr::firstIface() {
    IfaceLst.first();
}

/*
 * gets next interface on list
 */
SPtr<TIfaceIface> TIfaceMgr::getIface() {
    return IfaceLst.get();
}


/*
 * gets interface by it's name (or NULL if no such inteface exists)
 * @param name - interface name
 */
SPtr<TIfaceIface> TIfaceMgr::getIfaceByName(string name) {
    SPtr<TIfaceIface> ptr;
    IfaceLst.first();
    while ( ptr = IfaceLst.get() ) {
	if ( !strcmp(name.c_str(),ptr->getName()) )
	    return ptr;
    }
    return 0; // NULL
}

/*
 * gets interface by it ID (or NULL if no such interface exists)
 * @param id - interface id
 */ 
SPtr<TIfaceIface> TIfaceMgr::getIfaceByID(int id) {
    SPtr<TIfaceIface> ptr;
    IfaceLst.first();
    while ( ptr = IfaceLst.get() ) {
	if ( id == ptr->getID() )
	    return ptr;
    }
    return 0; //NULL
}

/*
 * gets interface by socket descriptor (or NULL if no such interface exists)
 */
SPtr<TIfaceIface> TIfaceMgr::getIfaceBySocket(int fd) {
    SPtr<TIfaceIface> ptr;
    IfaceLst.first();
    while ( ptr = IfaceLst.get() ) {
	if ( ptr->getSocketByFD(fd) )
	    return ptr;
    }
    return 0;
}

/*
 * tries to read data from any socket on all interfaces 
 * returns after time seconds.
 * @param time - listens for time seconds
 * @param buf - buffer
 * @param bufsize - buffer size
 * @param peer - informations about sender
 * returns socketID
 */
int TIfaceMgr::select(unsigned long time, char *buf, 
		      int &bufsize, SPtr<TIPv6Addr> peer) {
    struct timeval czas;
    int result;
    if (time > DHCPV6_INFINITY/2)
	time /=2;

#ifdef BSD
    // For some reason, Darwin kernel doesn't like too large timeout values
    if (time > DHCPV6_INFINITY/4)
        time = 3600*24*7; // a week is enough
#endif
    
    czas.tv_sec=time;
    czas.tv_usec=0;

    // tricks with FDS macros
    fd_set fds;
    fds = *TIfaceSocket::getFDS();

    int maxFD;
    //maxFD = FD_SETSIZE;
    maxFD = TIfaceSocket::getMaxFD() + 1;

    result = ::select(maxFD,&fds,NULL, NULL, &czas);

    // something received

    if (result==0) { // timeout, nothing received
        bufsize = 0;
        return 0; 
    }
    if (result<0) {
        char buf[512];
        strncpy(buf, strerror(errno),512);
        Log(Debug) << "Failed to read sockets (select() returned " << result << "), error=" << buf << LogEnd;
        return 0;
    }

    SPtr<TIfaceIface> iface;
    SPtr<TIfaceSocket> sock;
    bool found = 0;
    IfaceLst.first();
    while ( (!found) && (iface = IfaceLst.get()) ) {
	iface->firstSocket();
	while ( sock = iface->getSocket() ) {
	    if (FD_ISSET(sock->getFD(),&fds)) {
		found = true;
		break;
	    }	
	}
    }

    if (!found) {
        Log(Error) << "Seems like internal error. Unable to find any socket with incoming data." << LogEnd;
        return 0;
    }
    
    char myPlainAddr[48];   // my plain address
    char peerPlainAddr[48]; // peer plain address

    // receive data (pure C function used)
    result = sock_recv(sock->getFD(), myPlainAddr, peerPlainAddr, buf, bufsize);
    char peerAddrPacked[16];
    char myAddrPacked[16];
    inet_pton6(peerPlainAddr,peerAddrPacked);
    inet_pton6(myPlainAddr,myAddrPacked);
    peer->setAddr(peerAddrPacked);

    if (result==-1) {
        Log(Error) << "Socket recv() failure detected." << LogEnd;
        bufsize = 0;
	return -1;
    }

#ifndef WIN32
    // check if we've received data addressed to us. There's problem with sockets binding. 
    // If there are 2 open sockets (one bound to multicast and one to global address),
    // each packet sent on multicast address is also received on unicast socket.
    char anycast[16] = {0};
    if (!iface->flagLoopback() 
	&& memcmp(sock->getAddr()->getAddr(), myAddrPacked, 16)
	&& memcmp(sock->getAddr()->getAddr(), anycast, 16) ) {
	    Log(Debug) << "Received data on address " << myPlainAddr << ", expected " 
                   << *sock->getAddr() << ", message ignored." << LogEnd;
	    bufsize = 0;
	    return 0;
    }  
#endif

    bufsize = result;
    return sock->getFD();
}

/*
 * returns interface count
 */
int TIfaceMgr::countIface() {
	return IfaceLst.count();
}

/*
 * dump yourself to file
 */
void TIfaceMgr::dump()
{
    std::ofstream xmlDump;
    xmlDump.open(this->XmlFile.c_str());
    xmlDump << *this;
    xmlDump.close();
}

/*
 * destructor. Does really nothing. (SPtr is a sweet thing, isn't it?)
 */
TIfaceMgr::~TIfaceMgr()
{
}

string TIfaceMgr::printMac(char * mac, int macLen) {
    ostringstream tmp;
    int i;
    unsigned char x;

    for (i=0; i<macLen; i++) {
	if (i)
	    tmp << ":";
	tmp << hex;
	tmp.fill('0');
	tmp.width(2);
	x = (unsigned char) mac[i];
	tmp << (unsigned int)x;
    }
    return tmp.str();
}

/// adds parameter to parameters list
///
/// @param param pointer to table
/// @param offset offset in table
/// @param value value to be copied
///
/// @return next unused offset
///
int TIfaceMgr::addParam(char ** param, int offset, const char * value)
{
    param[offset] = new char[strlen(value)+1];
    strncpy(param[offset], value, strlen(value)+1);
    return ++offset;
}

void TIfaceMgr::freeParams(char ** param)
{
    int offset = 0;
    while (param[offset] != NULL) {
        delete [] param[offset];
        param[offset] = 0;
        offset++;
    }
}

void TIfaceMgr::notifyScripts(std::string scriptName, SPtr<TMsg> question, SPtr<TMsg> reply)
{
    if (!scriptName.length()) {
        Log(Debug) << "Not executing external script (Notify script disabled)." << LogEnd;
        return;
    }

    string action;

    char * params[512];
    char * env[512];
    int paramCnt = 0;
    int envCnt = 0;

    stringstream tmp;

    for (int i=0; i<512; i++) {
        params[i]=0;
        env[i]=0;
    }

    // get PATH
    char * path = getenv("PATH");
    if (path) {
        tmp << "PATH=" << path;
        envCnt = addParam(env, envCnt, tmp.str().c_str());
        tmp.str("");
    }

    switch (question->getType())
    {
    case REQUEST_MSG:
    case CONFIRM_MSG:
        action = "add";
        break;
    case RELEASE_MSG:
        action = "delete";
        break;
    case RENEW_MSG:

        break;
    default:
        Log(Debug) << "Script execution skipped for " << reply->getName() << " response to " << question->getName() << LogEnd;
        return;
    }


    paramCnt = addParam(params, paramCnt, scriptName.c_str());
    paramCnt = addParam(params, paramCnt, action.c_str());

    int ifindex = reply->getIface();
    SPtr<TIfaceIface> iface = (Ptr*)getIfaceByID(ifindex);
    if (!iface) {
        Log(Error) << "Unable to find interface with ifindex=" << ifindex << ". Script NOT called." << LogEnd;
        return;
    }

    tmp << "IFACE=" << iface->getName();
    envCnt = addParam(env, envCnt, tmp.str().c_str());

    tmp.str("");
    tmp << "IFINDEX=" << dec << (int)iface->getID();
    envCnt = addParam(env, envCnt, tmp.str().c_str());

    int ipCnt=1;
    int pdCnt=1;
    SPtr<TIPv6Addr> ip;

    reply->firstOption();
    while ( SPtr<TOpt> opt = reply->getOption() ) {
        switch (opt->getOptType()) {
        case OPTION_IA_NA:
        case OPTION_IA_TA:
            {
                opt->firstOption();
                while (SPtr<TOpt> subopt = opt->getOption()) {
                    if (subopt->getOptType() == OPTION_IAADDR) {
                        SPtr<TOptIAAddress> addr = (Ptr*) subopt;
                        tmp.str("");
                        tmp << "ADDR" << ipCnt++ << "=" << addr->getAddr()->getPlain() << " " << addr->getPref() << " " << addr->getValid();
                        envCnt = addParam(env, envCnt, tmp.str().c_str());
                    }
                }
                break;
            }
        case OPTION_IA_PD:
            {
                opt->firstOption();
                while (SPtr<TOpt> subopt = opt->getOption()) {
                    if (subopt->getOptType() == OPTION_IAPREFIX) {
                        SPtr<TOptIAPrefix> prefix = (Ptr*) subopt;
                        tmp.str("");
                        tmp << "PREFIX" << pdCnt++ << "=" << prefix->getPrefix()->getPlain() << " "
                            << int(prefix->getPrefixLength()) << " "
                            << prefix->getPref() << " " << prefix->getValid();
                        envCnt = addParam(env, envCnt, tmp.str().c_str());
                    }
                }
                break;
            }
        default:
            {
                tmp.str("");
                tmp << "OPTION" << opt->getOptType() << "=\"" << opt->getPlain() << "\"";
                envCnt = addParam(env, envCnt, tmp.str().c_str());
            }
        }
    }

    Log(Debug) << "About to execute " << scriptName << " script, " << paramCnt << " parameters, " << envCnt << " variables." << LogEnd;
    int returnCode = execute(scriptName.c_str(), params, env);
    freeParams(params);
    freeParams(env);

    Log(Info) << "Script execution complete, return code=" << returnCode << LogEnd;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------
/*
 * just flush this data in XML format (used for logging and debugging purposes only)
 */
ostream & operator <<(ostream & strum, TIfaceMgr &x)
{
    strum << "<IfaceMgr>" << endl;
    SPtr<TIfaceIface> ptr;
    x.IfaceLst.first();
    while ( ptr=x.IfaceLst.get() ) {
        strum << *ptr;
    }
    
    strum << "</IfaceMgr>" << endl;
    return strum;
}
