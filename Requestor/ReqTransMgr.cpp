/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: ReqTransMgr.cpp,v 1.1 2007-12-02 10:32:00 thomson Exp $
 */

#include "SocketIPv6.h"
#include "ReqTransMgr.h"
#include "ReqMsg.h"
#include "OptAddr.h"
#include "OptDUID.h"
#include "OptGeneric.h"
#include "Logger.h"
#include "ReqOpt.h"

ReqTransMgr::ReqTransMgr(TIfaceMgr * ifaceMgr)
{
    this->IfaceMgr = ifaceMgr;
}

void ReqTransMgr::SetParams(ReqCfgMgr * cfgMgr)
{
    this->CfgMgr = cfgMgr;
}

bool ReqTransMgr::BindSockets()
{
    if (!CfgMgr) {
        Log(Crit) << "Unable to bind sockets: no configration set." << LogEnd;
        return false;
    }

    Iface = IfaceMgr->getIfaceByName(CfgMgr->iface);
    if (!Iface) {
        Log(Crit) << "Unable to bind sockets: Interface " << CfgMgr->iface << " not found." << LogEnd;
        return false;
        this->Iface = Iface;
    }

    // get link-local address
    char* llAddr = 0;
    Iface->firstLLAddress();
    llAddr=Iface->getLLAddress();
    if (!llAddr) {
    	Log(Error) << "Interface " << Iface->getFullName() << " does not have link-layer address. Weird." << LogEnd;
    	return false;
    }

    SmartPtr<TIPv6Addr> ll = new TIPv6Addr(llAddr);

    if (!Iface->addSocket(ll, DHCPCLIENT_PORT, true, true)) {
        Log(Crit) << "Socket creation or binding failed." << LogEnd;
        return false;
    }
    Iface->firstSocket();
    Socket = Iface->getSocket();
    if (!Socket) {
        Log(Crit) << "No socket found. Something is wrong." << LogEnd;
        return false;
    }
    Log(Debug) << "Socket " << Socket->getFD() << " created on the " << Iface->getFullName() << " interface." << LogEnd;

    return true;    
}

bool ReqTransMgr::SendMsg()
{
    // TODO
    SPtr<TIPv6Addr> dstAddr = new TIPv6Addr("ff02::1:2", true);
    
    Log(Debug) << "Transmitting data on the " << Iface->getFullName() << " interface to " << dstAddr->getPlain() << " address." << LogEnd;
    TReqMsg * msg = new TReqMsg(Iface->getID(), dstAddr, LEASEQUERY_MSG);

    char buf[1024];
    int bufLen;
    memset(buf, 1024, 0xff);

    if (CfgMgr->addr) {
        Log(Debug) << "Creating ADDRESS-based query." << LogEnd;
        // Address based query
        buf[0] = QUERY_BY_ADDRESS;
        // buf[1..16] - link address, leave as ::
        memset(buf+1, 16, 0);
        bufLen = 17;

        // add new IAADDR option
        SPtr<TIPv6Addr> a = new TIPv6Addr(CfgMgr->addr, true);
        TReqOptAddr * optAddr = new TReqOptAddr(OPTION_IAADDR, a, msg);
        optAddr->storeSelf(buf+bufLen);

        bufLen += optAddr->getSize(); // +4, perhaps?
        free(optAddr);
        
    } else {
        Log(Debug) << "Creating DUID-based query." << LogEnd;
        // DUID based query
        buf[0] = QUERY_BY_CLIENTID;
        // buf[1..16] - link address, leave as ::
        memset(buf+1, 16, 0);
        bufLen = 17;

        printhex(buf, 48);
        SPtr<TDUID> duid = new TDUID(CfgMgr->duid);
        TReqOptDUID * optDuid = new TReqOptDUID(OPTION_CLIENTID, duid, msg);
        optDuid->storeSelf(buf+bufLen);
        bufLen += optDuid->getSize(); // +4, perhaps?
        free(optDuid);
    }
    SPtr<TOpt> opt = new TReqOptGeneric(OPTION_LQ_QUERY, buf, bufLen, msg);
    msg->addOption(opt);
    
    char msgbuf[1024];
    int  msgbufLen;
    memset(msgbuf, 1024, 0xff);

    msgbufLen = msg->storeSelf(msgbuf);

    Log(Debug) << msg->getSize() << "-byte long message prepared." << LogEnd;

    if (this->Socket->send(msgbuf, msgbufLen, dstAddr, DHCPSERVER_PORT)<0) {
        Log(Error) << "Message transmission failed." << LogEnd;
        return false;
    }
    Log(Info) << "LQ_QUERY message sent." << LogEnd;
    return true;
}

bool ReqTransMgr::WaitForRsp()
{
    char buf[1024];
    int bufLen;
    SPtr<TIPv6Addr> sender = new TIPv6Addr();

    int sockFD;
    Log(Debug) << "Wating " << CfgMgr->timeout << " seconds for reply reception." << LogEnd;
    sockFD = this->IfaceMgr->select(CfgMgr->timeout, buf, bufLen, sender);

    SPtr<TIfaceSocket> sock;
    if (sockFD>=0) {
        sock = Iface->getSocketByFD(sockFD);
        if (!sock) {
            Log(Error) << "Internal error. Unable to obtain socket." << LogEnd;
        }
        bufLen = sock->recv(buf, sender);

        PrintRsp(buf, bufLen);
        return true;
    } else {
        Log(Error) << "Response not received. Timeout or socket error." << LogEnd;
        return false;
    }
    
}

void ReqTransMgr::PrintRsp(char * buf, int bufLen)
{
    Log(Notice) << bufLen << " bytes received." << LogEnd;
    printhex(buf, bufLen);
}
