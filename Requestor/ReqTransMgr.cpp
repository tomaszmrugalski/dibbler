/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 */

#include <stdio.h>
#include <string.h>
#include <sstream>
#include "SocketIPv6.h"
#include "ReqTransMgr.h"
#include "ReqMsg.h"
#include "OptAddr.h"
#include "OptDUID.h"
#include "OptGeneric.h"
#include "Logger.h"
#include "ReqOpt.h"
#include "Portable.h"
#include "hex.h"

using namespace std;

ReqTransMgr::ReqTransMgr(TIfaceMgr * ifaceMgr)
    :CfgMgr(NULL)
{
    IfaceMgr = ifaceMgr;
}

void ReqTransMgr::SetParams(ReqCfgMgr * cfgMgr)
{
    CfgMgr = cfgMgr;
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
    }

#ifndef WIN32
    Log(Info) << "Binding socket on loopback interface." << LogEnd;

    SPtr<TIfaceIface> ptrIface;
    SPtr<TIfaceIface> loopback;
    IfaceMgr->firstIface();
    while (ptrIface=IfaceMgr->getIface()) {
        if (!ptrIface->flagLoopback()) {
            continue;
	    }
	    loopback = ptrIface;
	    break;
    }
    if (!loopback) {
	   Log(Crit) << "Loopback interface not found!" << LogEnd;
	   return false;
    }

    SPtr<TIPv6Addr> loopAddr = new TIPv6Addr("::", true);
    Log(Notice) << "Creating control (" << *loopAddr << ") socket on the " << loopback->getName() 
		<< "/" << loopback->getID() << " interface." << LogEnd;

    if (!loopback->addSocket(loopAddr,DHCPCLIENT_PORT, false, true)) {
	Log(Crit) << "Proper socket creation failed." << LogEnd;
	return false;
    }

    loopback->firstSocket();
    Socket = loopback->getSocket();
    if (!Socket) {
        Log(Crit) << "No socket found. Something is wrong." << LogEnd;
        return false;
    }
    Log(Debug) << "Socket " << Socket->getFD() << " created on the " << loopback->getFullName() << " interface." << LogEnd;


#endif

    // get link-local address
    char* llAddr = 0;
    Iface->firstLLAddress();
    llAddr=Iface->getLLAddress();
    if (!llAddr) {
    	Log(Error) << "Interface " << Iface->getFullName() << " does not have link-layer address. Weird." << LogEnd;
    	return false;
    }

    SPtr<TIPv6Addr> ll = new TIPv6Addr(llAddr);

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

    SPtr<TIPv6Addr> dstAddr;
    if (!CfgMgr->dstaddr)
	dstAddr = new TIPv6Addr("ff02::1:2", true);
    else
	dstAddr = new TIPv6Addr(CfgMgr->dstaddr, true);
    
    Log(Debug) << "Transmitting data on the " << Iface->getFullName() << " interface to " 
	       << dstAddr->getPlain() << " address." << LogEnd;
    TReqMsg * msg = new TReqMsg(Iface->getID(), dstAddr, LEASEQUERY_MSG);

    char buf[1024];
    int bufLen;
    memset(buf, 1024, 0xff);

    if (CfgMgr->addr) {
        Log(Debug) << "Creating ADDRESS-based query. Asking for " << CfgMgr->addr << " address." << LogEnd;
        // Address based query
        buf[0] = QUERY_BY_ADDRESS;
        // buf[1..16] - link address, leave as ::
        memset(buf+1, 0, 16);
        bufLen = 17;

        // add new IAADDR option
        SPtr<TIPv6Addr> a = new TIPv6Addr(CfgMgr->addr, true);
        TReqOptAddr * optAddr = new TReqOptAddr(OPTION_IAADDR, a, msg);
        optAddr->storeSelf(buf+bufLen);
        bufLen += optAddr->getSize();
        delete optAddr;
        
    } else {
        Log(Debug) << "Creating DUID-based query. Asking for " << CfgMgr->duid << " DUID." << LogEnd;
        // DUID based query
        buf[0] = QUERY_BY_CLIENTID;
        // buf[1..16] - link address, leave as ::
        memset(buf+1, 0, 16);
        bufLen = 17;

        SPtr<TDUID> duid = new TDUID(CfgMgr->duid);
        TReqOptDUID * optDuid = new TReqOptDUID(OPTION_CLIENTID, duid, msg);
        optDuid->storeSelf(buf+bufLen);
        bufLen += optDuid->getSize();

        delete optDuid;
    }

    SPtr<TDUID> clientDuid = new TDUID("00:01:00:01:0e:ec:13:db:00:02:02:02:02:02");
    SPtr<TOpt> opt = new TReqOptDUID(OPTION_CLIENTID, clientDuid, msg);
    msg->addOption(opt);

    opt = new TReqOptGeneric(OPTION_LQ_QUERY, buf, bufLen, msg);
    msg->addOption(opt);
    
    char msgbuf[1024];
    int  msgbufLen;
    memset(msgbuf, 0xff, 1024);

    msgbufLen = msg->storeSelf(msgbuf);

    Log(Debug) << msg->getSize() << "-byte long LQ_QUERY message prepared." << LogEnd;

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
    int bufLen = 1024;
    memset(buf, 0, bufLen);
    SPtr<TIPv6Addr> sender = new TIPv6Addr();
    SPtr<TIPv6Addr> myaddr(new TIPv6Addr());

    int sockFD;
    Log(Debug) << "Waiting " << CfgMgr->timeout << " seconds for reply reception." << LogEnd;
    sockFD = this->IfaceMgr->select(CfgMgr->timeout, buf, bufLen, sender, myaddr);
    
    Log(Debug) << "Returned socketID=" << sockFD << LogEnd;
    if (sockFD>0) {
        Log(Info) << "Received " << bufLen << " bytes response." << LogEnd;
        PrintRsp(buf, bufLen);
    } else {
        Log(Error) << "Response not received. Timeout or socket error." << LogEnd;
        return false;
    }
    
    return true;
}

void ReqTransMgr::PrintRsp(char * buf, int bufLen)
{
    if (bufLen < 4) {
        Log(Error) << "Unable to print message: truncated (min. len=4 required)." << LogEnd;
    }
    
    int msgType = buf[0];
    int transId = buf[1] + 256*buf[2] + 256*256*buf[3];

    Log(Info) << "MsgType: " << msgType << ", transID=0x" << hex << transId << dec << LogEnd;

    ParseOpts(msgType, 0, buf+4, bufLen-4);
}

bool ReqTransMgr::ParseOpts(int msgType, int recurseLevel, char * buf, int bufLen)
{
    std::ostringstream o;
    for (int i=0; i<recurseLevel; i++)
	o << "  ";
    string linePrefix = o.str();

    int pos = 0;
    SPtr<TOpt> ptr;
    bool print = true;
    while (pos<bufLen) {
	if (pos+4>bufLen) {
	    Log(Error) << linePrefix << "Message " << msgType << " truncated. There are " << (bufLen-pos) 
		       << " bytes left to parse. Bytes ignored." << LogEnd;
	    return false;
	}
	unsigned short code = readUint16(buf+pos);
	pos += sizeof(uint16_t);
	unsigned short length = readUint16(buf+pos);
	pos += sizeof(uint16_t);
	if (pos+length>bufLen) {
	    Log(Error) << linePrefix << "Truncated option (type=" << code << ", len=" << length 
		       << " received in message << " << msgType << ". Option ignored." << LogEnd;
	    pos += length;
	    continue;
	}
	
	if (!allowOptInMsg(msgType,code)) {
	    Log(Warning) << linePrefix << "Invalid option received: Option " << code << " not allowed in message type " 
			 << msgType << ". Ignored." << LogEnd;
	    pos+=length;
	    continue;
	}
	if (!recurseLevel && !allowOptInOpt(msgType,0,code)) {
	    Log(Warning) << "Invalid option received: Option " << code << " not allowed in message type "<< msgType 
			 << " as a base option (as suboption only permitted). Ignored." << LogEnd;
	        pos+=length;
	        continue;
	}
	
        string name, o;
        o = "";
        name = "";
	
	switch (code) {
        case OPTION_STATUS_CODE:
	{
	    name ="Status Code";
	    unsigned int st = buf[pos]*256 + buf[pos+1];
	    
	    char *Message = new char[length+10];
	    memcpy(Message,buf+pos+2,length-2);
	    sprintf(Message+length-2, "(%u)", st);
	    o = string(Message);
	    delete [] Message;
	    break;
	}
	case OPTION_CLIENTID:
	    name = "ClientID";
	    o = BinToString(buf+pos, length);
	    break;
	case OPTION_SERVERID:
	    name = "ServerID";
	    o = BinToString(buf+pos, length);
	    break;
	case OPTION_LQ_QUERY:
	    name = "LQ Query Option";
	    break;
	case OPTION_CLIENT_DATA:
	    name = "LQ Client Data Option";
	    Log(Info) << linePrefix << "Option " << name << "(code=" << code << "), len=" << length << LogEnd;
	    ParseOpts(msgType, recurseLevel+1, buf+pos, length);
	    print = false;
	    break;
	case OPTION_CLT_TIME:
	{
            name = "LQ Client Last Transmission Time";
	    unsigned int t = readUint32(buf+pos);
	    ostringstream out;
	    out << t << " second(s)";
	    o = out.str();
	    break;
	}
	case OPTION_LQ_RELAY_DATA:
            name = "LQ Relay Data";
	    break;
	case OPTION_LQ_CLIENT_LINK:
            name = "LQ Client Link";
	    break;
	case OPTION_IAADDR:
	{
	    TIPv6Addr * addr = new TIPv6Addr(buf+pos, false);
	    unsigned int pref  = readUint32(buf+pos+16);
	    unsigned int valid = readUint32(buf+pos+20);
	    name = "IAADDR";
	    ostringstream out;
	    out << "addr=" << addr->getPlain() << ", pref=" << pref << ", valid=" << valid;
	    o = out.str();
	    break;
	}

	default:
	    break;
	}
	if (print)
	    Log(Info) << linePrefix << "Option " << name << "(code=" << code << "), len=" << length << ": " << o << LogEnd;
	print = true;
        pos+=length;
    }
    
    return true;
}

string ReqTransMgr::BinToString(char * buf, int bufLen)
{
    return (hexToText((uint8_t*)buf, bufLen, true));
}

