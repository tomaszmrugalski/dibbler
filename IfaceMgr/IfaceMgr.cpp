/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <errno.h>
#include "Portable.h"
#include "IfaceMgr.h"
#include "Iface.h"
#include "SocketIPv6.h"
#include "Logger.h"
#include "Msg.h"
#include "OptIAAddress.h"
#include "OptIAPrefix.h"
#include "ScriptParams.h"

using namespace std;

/// constructor
///
/// @param xmlFile xml file, where interface info will be stored
/// @param getIfaces  specifies if interfaces should be detected
TIfaceMgr::TIfaceMgr(const std::string& xmlFile, bool getIfaces)
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
        iface->setMBit(ptr->m_bit);
        iface->setOBit(ptr->o_bit);
        this->IfaceLst.append(iface);
        ptr = ptr->next;
    }
    if_list_release(ifaceList); // allocated in pure C, and so release it there

    dump();
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
SPtr<TIfaceIface> TIfaceMgr::getIfaceByName(const std::string& name) {
    SPtr<TIfaceIface> ptr;
    IfaceLst.first();
    while ( ptr = IfaceLst.get() ) {
        if ( !strcmp(name.c_str(),ptr->getName()) )
            return ptr;
    }
    return SPtr<TIfaceIface>(); // NULL
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
    return SPtr<TIfaceIface>(); // NULL
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
    return SPtr<TIfaceIface>(); // NULL
}

/// tries to read data from any socket on all interfaces
/// returns after time seconds.
/// @param time listens for time seconds
/// @param buf buffer
/// @param bufsize buffer size
/// @param peer [out] sender address
/// @param myaddr [out] local IPv6 address
///
/// @return socket descriptor (or negative values for errors)
int TIfaceMgr::select(unsigned long time, char *buf,
                      int &bufsize, SPtr<TIPv6Addr> peer,
                      SPtr<TIPv6Addr> myaddr) {
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
    maxFD = TIfaceSocket::getMaxFD() + 1;

    // no sockets to listen  on... hopefully this is just inactive mode,
    // not an error
    if (!TIfaceSocket::getCount()) {
        Log(Debug) << "No sockets open. Sleeping for " << time << " seconds." << LogEnd;
#ifdef WIN32
        Sleep(time*1000); // Windows sleep is specified in milliseconds
#else
        sleep(time); // Posix sleep is specified in seconds
#endif
        return 0;
    }
    result = ::select(maxFD, &fds, NULL, NULL, &czas);

    // something received

    if (result==0) { // timeout, nothing received
        bufsize = 0;
        return -1;
    }
    if (result<0) {
        char buf[512];
        strncpy(buf, strerror(errno),512);
        Log(Debug) << "Failed to read sockets (select() returned " << result
                   << "), error=" << buf << LogEnd;
        return -1;
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
        Log(Error) << "Internal error. Can't find any socket with incoming data." << LogEnd;
        return -1;
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
    myaddr->setAddr(myAddrPacked);

    if (result==-1) {
        Log(Error) << "Socket recv() failure detected." << LogEnd;
        bufsize = 0;
        return -1;
    }

#ifdef MOD_SRV_DST_ADDR_CHECK
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
            return -1;
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
    closeSockets();
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

void TIfaceMgr::optionToEnv(TNotifyScriptParams& params, SPtr<TOpt> opt, std::string txtPrefix )
{
    switch (opt->getOptType()) {
    case OPTION_IA_NA:
    case OPTION_IA_TA:
    {
        opt->firstOption();
        while (SPtr<TOpt> subopt = opt->getOption()) {
            if (subopt->getOptType() == OPTION_IAADDR) {
                SPtr<TOptIAAddress> addr = (Ptr*) subopt;
                params.addAddr(addr->getAddr(), addr->getPref(), addr->getValid(), txtPrefix);
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
                params.addPrefix(prefix->getPrefix(),
                                 prefix->getPrefixLength(),
                                 prefix->getPref(),
                                 prefix->getValid());
            }
        }
        break;
    }
    case OPTION_NEXT_HOP: {
        if (opt->countOption()) {
            // suboptions defined
            opt->firstOption();
            while (SPtr<TOpt> subopt = opt->getOption()) {
                if (subopt->getOptType() != OPTION_RTPREFIX)
                    continue; // ignore other options
                params.addParam("OPTION_NEXT_HOP_RTPREFIX", opt->getPlain() + " " + subopt->getPlain());
            }
        } else {
            // no suboptions, just NEXT_HOP (default router, without ::/0 route specified)
            // Will define something like this: OPTION_NEXT_HOP=2001:db8:1::1
            params.addParam("OPTION_NEXT_HOP", opt->getPlain());
        }
        break;
    }
    case OPTION_RTPREFIX: {
        params.addParam("OPTION_RTPREFIX", opt->getPlain());
        break;
    }
    default: {
      stringstream tmp;
      if (txtPrefix.length()) {
        tmp << txtPrefix << "_";
      }
      tmp << "OPTION" << opt->getOptType();
      params.addParam(tmp.str().c_str(), opt->getPlain());
      break;
    }
    }
}



void TIfaceMgr::notifyScripts(const std::string& scriptName, SPtr<TMsg> question, SPtr<TMsg> reply)
{
    TNotifyScriptParams* params = (TNotifyScriptParams*)reply->getNotifyScriptParams();
    if (params) {
        notifyScripts(scriptName, question, reply, *params);
    } else {
        TNotifyScriptParams par;
        notifyScripts(scriptName, question, reply, par);
    }
}

void TIfaceMgr::notifyScript(const std::string& scriptName, std::string action,
                             TNotifyScriptParams& params) {
    const char * argv[3];

    // get PATH
    char * path = getenv("PATH");
    if (path) {
        params.addParam("PATH", string(path));
    }

    // parameters: [0] - script name, [1] - action (add, modify, delete)
    argv[0] = scriptName.c_str();
    argv[1] = action.c_str();
    argv[2] = NULL;

    Log(Debug) << "About to execute " << scriptName << " script, "
               << params.envCnt << " variables." << LogEnd;
    int returnCode = execute(scriptName.c_str(), argv, params.env);

    if (returnCode>=0) {
        Log(Debug) << "Script execution complete, return code=" << returnCode << LogEnd;
    } else {
        // negative return code, something went wrong
        Log(Warning) << "Script execution failed, return code=" << returnCode << LogEnd;
    }
}

void TIfaceMgr::notifyScripts(const std::string& scriptName, SPtr<TMsg> question,
                              SPtr<TMsg> reply, TNotifyScriptParams& params)
{
    if (!scriptName.length()) {
        Log(Debug) << "Not executing external script (Notify script disabled)." << LogEnd;
        return;
    }

    stringstream tmp;
    string action;

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
    case REBIND_MSG:
    case INFORMATION_REQUEST_MSG:
        action = "update";
        break;
    default:
        Log(Debug) << "Script execution skipped for " << reply->getName() << " response to "
                   << question->getName() << ". No action needed for this message type." << LogEnd;
        return;
    }

    int ifindex = reply->getIface();
    SPtr<TIfaceIface> iface = (Ptr*)getIfaceByID(ifindex);
    if (!iface) {
        Log(Error) << "Unable to find interface with ifindex=" << ifindex
                   << ". Script NOT called." << LogEnd;
        return;
    }

    params.addParam("IFACE", iface->getName());

    tmp << dec << (int)iface->getID();
    params.addParam("IFINDEX", tmp.str().c_str());
    tmp.str("");

    string remote;
    if (reply->getRemoteAddr()) {
      remote = reply->getRemoteAddr()->getPlain();
    } else {
      remote = string(ALL_DHCP_RELAY_AGENTS_AND_SERVERS);
    }

    params.addParam("REMOTE_ADDR", remote);

    params.addParam("CLNT_MESSAGE", question->getName());

    params.addParam("SRV_MESSAGE", reply->getName());

    // add options from server REPLY
    reply->firstOption();
    while ( SPtr<TOpt> opt = reply->getOption() ) {
        optionToEnv(params, opt, "SRV");
    }

    // add options from client message
    question->firstOption();
    while( SPtr<TOpt> opt = question->getOption() ) {
        optionToEnv(params, opt, "CLNT");
    }

    notifyScript(scriptName, action, params);
}

/// @brief closes all sockets
void TIfaceMgr::closeSockets() {
    Log(Debug) << "Closing all sockets." << LogEnd;
    firstIface();
    while (SPtr<TIfaceIface> iface = getIface()) {
        iface->firstSocket();

        while (SPtr<TIfaceSocket> socket = iface->getSocket()) {
            iface->delSocket(socket->getFD());
        }
    }
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
