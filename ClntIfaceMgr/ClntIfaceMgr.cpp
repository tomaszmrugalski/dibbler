/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceMgr.cpp,v 1.3 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#include "Portable.h"
#include "SmartPtr.h"
#include "ClntIfaceMgr.h"
#include "Msg.h"
#include "ClntMsgReply.h"
#include "ClntMsgAdvertise.h"
#include "Logger.h"
using namespace logger;
using namespace std;

bool TClntIfaceMgr::sendUnicast(int iface, char *msg, int size, SmartPtr<TIPv6Addr> addr)
{
    // FIXME:
    return true;
}

bool TClntIfaceMgr::sendMulticast(int iface, char * msg, int msgsize)
{
    // prepare address
    char addr[16];
    inet_pton6(ALL_DHCP_RELAY_AGENTS_AND_SERVERS,addr);

    // get interface
    SmartPtr<TIfaceIface> Iface;
    Iface = this->getIfaceByID(iface);
    if (!Iface) {
        std::clog << logError << " No such interface (id=" << iface << "). Send failed." << logger::endl;
        return false;
    }

    // are there any sockets on this interface?
    SmartPtr<TIfaceSocketIPv6> sock; 
    if (! Iface->countSocket() ) {
        std::clog << logError << "Interface " << Iface->getName() 
            << " has no open sockets" << logger::endl;
        return false;
    }

    // yes, there are. Get first of them (there should by only one anyway :)
    Iface->firstSocket();
    sock = Iface->getSocket();

    // FIXME: check if send() has finished successfully
    sock->send( (char*)msg, msgsize, new TIPv6Addr(addr), DHCPSERVER_PORT);

    return true;
}


SmartPtr<TMsg> TClntIfaceMgr::select(unsigned int timeout)
{
    int bufsize=4096;
    static char buf[4096];
    SmartPtr<TIPv6Addr> peer(new TIPv6Addr());
    int sockid;
    int msgtype;
    int ifaceid;

    sockid = TIfaceMgr::select(timeout, buf, bufsize, peer);

    if (sockid>0) {
        if (bufsize<4) {
			if (buf[0]!=CONTROL_MSG) {
				clog << logWarning << "Received message is too short (" << bufsize
					 << ") bytes." << logger::endl;
			} else {
				clog << logWarning << "Control message received." << logger::endl;
			}
            return SmartPtr<TMsg>(); // NULL
        }
        msgtype = buf[0];
        SmartPtr<TMsg> ptr;
        SmartPtr<TIfaceIface> ptrIface;
        ptrIface = this->getIfaceBySocket(sockid);
        ifaceid = ptrIface->getID();
        std::clog << logNotice << "Received " << bufsize << " bytes via " << sockid 
            << " socket, ifaceid=" << ifaceid << ", msgtype=" << msgtype << ")" << logger::endl;

        switch (msgtype) {
        case ADVERTISE_MSG:
            ptr = new TClntMsgAdvertise(That, ClntTransMgr, ClntCfgMgr, ClntAddrMgr,
                ifaceid,peer,buf,bufsize);
            return ptr;
        case SOLICIT_MSG:
        case REQUEST_MSG:
        case CONFIRM_MSG:
        case RENEW_MSG:
        case REBIND_MSG:
        case RELEASE_MSG:
        case DECLINE_MSG:
        case INFORMATION_REQUEST_MSG:
            std::clog << logWarning << "Illegal message type " << msgtype << " received." << logger::endl;
            return SmartPtr<TMsg>(); // NULL
        case REPLY_MSG:
            ptr = new TClntMsgReply(That, ClntTransMgr, ClntCfgMgr, ClntAddrMgr,
                ifaceid, peer,buf, bufsize);
            return ptr;

        case RECONFIGURE_MSG:
            //FIXME: Reconfigure message
            std::clog << logDebug << "Reconfigure Message is currently not supported." << logger::endl;
            return SmartPtr<TMsg>();
        case RELAY_FORW:
        case RELAY_REPL:
        default:
            std::clog << logWarning << "Message type " << msgtype << " not supported." << logger::endl;
            return SmartPtr<TMsg>();
        }
    } else {
        return SmartPtr<TMsg>();
    }
}

TClntIfaceMgr::TClntIfaceMgr() : TIfaceMgr()
{
}
//I think this that setting, can be eliminated, when received message, won't
//be executing any actions and I think that is possible
void TClntIfaceMgr::setThats(SmartPtr<TClntIfaceMgr> clntIfaceMgr,
                             SmartPtr<TClntTransMgr> clntTransMgr,
                             SmartPtr<TClntCfgMgr> clntCfgMgr,
                             SmartPtr<TClntAddrMgr> clntAddrMgr)
{
    ClntCfgMgr=clntCfgMgr;
    ClntAddrMgr=clntAddrMgr;
    ClntTransMgr=clntTransMgr;
    That=clntIfaceMgr;
}
