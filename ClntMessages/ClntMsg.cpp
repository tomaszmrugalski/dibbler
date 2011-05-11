/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsg.cpp,v 1.42 2008-11-13 22:40:26 thomson Exp $
 */

#ifdef WIN32
#include <winsock2.h>
#endif

#ifdef LINUX
#include <netinet/in.h>
#endif

#include <cmath>
#include <sstream>

#include "ClntCfgMgr.h"

#include "ClntMsg.h"
#include "ClntTransMgr.h"

#include "OptGeneric.h"
#include "OptEmpty.h"
#include "OptAddrLst.h"
#include "OptAddr.h"
#include "OptDUID.h"
#include "ClntOptIA_NA.h"
#include "ClntOptIA_PD.h"
#include "ClntOptTA.h"
#include "ClntOptOptionRequest.h"
#include "ClntOptPreference.h"
#include "ClntOptElapsed.h"
#include "ClntOptStatusCode.h"
#include "ClntOptDNSServers.h"
#include "ClntOptDomainName.h"
#include "ClntOptNTPServers.h"
#include "ClntOptTimeZone.h"
#include "ClntOptSIPServer.h"
#include "ClntOptSIPDomain.h"
#include "ClntOptFQDN.h"
#include "ClntOptNISServer.h"
#include "ClntOptNISDomain.h"
#include "ClntOptNISPServer.h"
#include "ClntOptNISPDomain.h"
#include "ClntOptLifetime.h"

#ifndef MOD_DISABLE_AUTH
#include "ClntOptAAAAuthentication.h"
#include "ClntOptKeyGeneration.h"
#include "ClntOptAuthentication.h"
#endif

#include "Logger.h"

static string msgs[] = { "",
		  "SOLICIT",
		  "ADVERTISE",
		  "REQUEST",
		  "CONFIRM",
		  "RENEW",
		  "REBIND",
		  "REPLY",
		  "RELEASE",
		  "DECLINE",
		  "RECONFIGURE",
		  "INF-REQUEST",
		  "RELAY-FORWARD"
		  "RELAY-REPLY",
	  "LEASEQUERY",
	  "LEASEQUERY-REPLY"};

void TClntMsg::invalidAllowOptInMsg(int msg, int opt) {
    string name;
    if (msg<=15)
	name = msgs[msg];
    Log(Warning) << "Option " << opt << " is not allowed in the " << name
		 << " message. Ignoring." << LogEnd;
}

void TClntMsg::invalidAllowOptInOpt(int msg, int parentOpt, int childOpt) {
    string name;
    if (msg<=15)
	name = msgs[msg];

    if (parentOpt==0)
	Log(Warning) << "Option " << childOpt << " is not allowed directly in " << name
		     << " message. Ignoring." << LogEnd;
    else
	Log(Warning) << "Option " << childOpt << " is not allowed in the " << parentOpt
		     << " in the " << name << " message. Ignoring." << LogEnd;
}

/**
 * creates message, based on a received buffer
 *
 * @param IfaceMgr
 * @param TransMgr
 * @param CfgMgr
 * @param AddrMgr
 * @param iface
 * @param addr
 * @param buf
 * @param bufSize
 */
TClntMsg::TClntMsg(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSize)
		   :TMsg(iface, addr, buf, bufSize)
{
    setDefaults();

    //After reading message code and transactionID
    //read options contained in message
    int pos=0;
    SPtr<TOpt> ptr;

    while (pos<bufSize) {
	if (pos+4>bufSize) {
	    Log(Error) << "Message " << MsgType << " truncated. There are " << (bufSize-pos)
		       << " bytes left to parse. Bytes ignored." << LogEnd;
	    break;
	}
	unsigned short code   = ntohs( *((unsigned short*) (buf+pos)));
	pos+=2;
	unsigned short length = ntohs( *((unsigned short*) (buf+pos)));
	pos+=2;
	if (pos+length>bufSize) {
	    Log(Error) << "Invalid option (type=" << code << ", len=" << length
		       << " received (msgtype=" << MsgType << "). Message dropped." << LogEnd;
	    IsDone = true;
	    return;
	}

	if (!allowOptInMsg(MsgType,code)) {
	    this->invalidAllowOptInMsg(MsgType, code);
	    pos+=length;
	    continue;
	}
	if (!allowOptInOpt(MsgType,0,code)) {
	    this->invalidAllowOptInOpt(MsgType, 0, code);
	    pos+=length;
	    continue;
	}
	ptr = 0;

	switch (code) {
	case OPTION_CLIENTID:
	case OPTION_SERVERID:
	    ptr = new TOptDUID(code, buf+pos,length,this);
	    break;
	case OPTION_IA_NA:
	    ptr = new TClntOptIA_NA(buf+pos,length,this);
	    break;
	case OPTION_IA_PD:
	    ptr = new TClntOptIA_PD(buf+pos,length,this);
	    break;
	case OPTION_ORO:
	    ptr = new TClntOptOptionRequest(buf+pos,length,this);
	    break;
	case OPTION_PREFERENCE:
	    ptr = new TClntOptPreference(buf+pos,length,this);
	    break;
	case OPTION_ELAPSED_TIME:
	    ptr = new TClntOptElapsed(buf+pos,length,this);
	    break;
	case OPTION_UNICAST:
	    ptr = new TOptAddr(OPTION_UNICAST,buf+pos,length,this);
	    break;
	case OPTION_STATUS_CODE:
	    ptr = new TClntOptStatusCode(buf+pos,length,this);
	    break;
	case OPTION_RAPID_COMMIT:
	    ptr = new TOptEmpty(code, buf+pos,length,this);
	    break;
	case OPTION_DNS_SERVERS:
	    ptr = new TClntOptDNSServers(buf+pos,length,this);
	    break;
	case OPTION_SNTP_SERVERS:
	    ptr = new TClntOptNTPServers(buf+pos,length,this);
	    break;
	case OPTION_DOMAIN_LIST:
	    ptr = new TClntOptDomainName(buf+pos, length, this);
	    break;
	case OPTION_NEW_TZDB_TIMEZONE:
	    ptr = new TClntOptTimeZone(buf+pos, length,this);
	    break;
	case OPTION_SIP_SERVER_A:
	    ptr = new TClntOptSIPServers(buf+pos, length, this);
	    break;
	case OPTION_SIP_SERVER_D:
	    ptr = new TClntOptSIPDomain(buf+pos, length, this);
	    break;
	case OPTION_NIS_SERVERS:
	    ptr = new TClntOptNISServers(buf+pos, length, this);
	    break;
	case OPTION_NIS_DOMAIN_NAME:
	    ptr = new TClntOptNISDomain(buf+pos, length, this);
	    break;
	case OPTION_NISP_SERVERS:
	    ptr = new TClntOptNISPServers(buf+pos, length, this);
	    break;
	case OPTION_NISP_DOMAIN_NAME:
	    ptr = new TClntOptNISPDomain(buf+pos, length, this);
	    break;
	case OPTION_FQDN:
	    ptr = new TClntOptFQDN(buf+pos, length, this);
	    break;
	case OPTION_INFORMATION_REFRESH_TIME:
	    ptr = new TClntOptLifetime(buf+pos, length, this);
	    break;
	case OPTION_AFTR_NAME:
	    ptr = new TOptString(code, buf+pos, length, this);
	    break;
	case OPTION_IA_TA: {
	    ptr = new TClntOptTA(buf+pos, length, this);
	    break;
	}
#ifndef MOD_DISABLE_AUTH
	case OPTION_AAAAUTH:
	    Log(Warning) << "Client is not supposed to receive OPTION_AAAAUTH, ignoring." << LogEnd;
	    break;
	case OPTION_KEYGEN:
	    ptr = new TClntOptKeyGeneration(buf+pos, length, this);
	    break;
	case OPTION_AUTH:
	    if (ClntCfgMgr().getAuthEnabled()) {
		this->DigestType = ClntCfgMgr().getDigest();
		ptr = new TClntOptAuthentication(buf+pos, length, this);
	    }
	    break;
#endif
	case OPTION_VENDOR_OPTS: {
	    ptr = new TOptVendorSpecInfo(code, buf+pos, length, this);
	    break;
	}
	case OPTION_RECONF_ACCEPT:
	case OPTION_USER_CLASS:
	case OPTION_VENDOR_CLASS:
	case OPTION_RECONF_MSG:
	case OPTION_RELAY_MSG:
	case OPTION_INTERFACE_ID:
	    Log(Warning) << "Option " << code<< " in message "
			 << MsgType << " is not supported." << LogEnd;
	    break;
	default:
	    ptr = parseExtraOption(buf+pos, code, length);
	    if (!ptr)
	    {
		Log(Warning) << "Unknown option: " << code << ", length=" << length
			     << ". Ignored." << LogEnd;
		pos+=length;
		continue;
	    }

	}

	if ( (ptr) && (ptr->isValid()) ) {
	    Options.push_back( ptr );
	} else {
	    Log(Warning) << "Option " << code << " is invalid. Ignoring." << LogEnd;
	}
	pos+=length;
    }

    SPtr<TOptDUID> optSrvID = (Ptr*)this->getOption(OPTION_SERVERID);
    if (!optSrvID) {
	Log(Warning) << "Message " << this->MsgType
		     << " does not contain SERVERID option. Ignoring." << LogEnd;
	this->IsDone = true;
	return;
    }

    this->firstOption();
    SPtr<TOpt> opt;
    while ( opt = getOption() )
	opt->setDUID(optSrvID->getDUID());

}

SPtr<TOpt> TClntMsg::parseExtraOption(const char *buf, unsigned int code, unsigned int length)
{
    SPtr<TOpt> ptr;
    SPtr<TClntCfgIface> cfgIface = TClntCfgMgr::instance().getIface(Iface);
    TClntCfgIface::TOptionStatusLst ExtraOpts = cfgIface->getExtraOptions();
    for (TClntCfgIface::TOptionStatusLst::iterator exp = ExtraOpts.begin();
	 exp != ExtraOpts.end();
	 ++exp) {
	if (code != (*exp)->OptionType)
	    continue;
	stringstream tmp;
	tmp << "Received expected extra option: type=" << code
	    << ", size=" << length << ", layout=";

	switch ( (*exp)->Layout)
	{
	case TOpt::Layout_Addr:
	{
	    ptr = new TOptAddr(code, buf, length, this);
	    Log(Info) << tmp.str() << "single-address" << LogEnd;
	    break;
	}
	case TOpt::Layout_AddrLst:
	{
	    ptr = new TOptAddrLst(code, buf, length, this);
	    Log(Info) << tmp.str() << "list-of-addesses" << LogEnd;
	    break;
	}
	case TOpt::Layout_String:
	{
	    ptr = new TOptString(code, buf, length, this);
	    Log(Info) << tmp.str() << "single string" << LogEnd;
	    break;
	}
	case TOpt::Layout_StringLst:
	{
	    ptr = new TOptStringLst(code, buf, length, this);
	    Log(Info) << tmp.str() << "list-of-strings" << LogEnd;
	    break;
	}
	case TOpt::Layout_Generic:
	default:
	{
	    ptr = new TOptGeneric(code, buf, length, this);
	    Log(Info) << tmp.str() << "generic" << LogEnd;
	    break;
	}
	}
    }

    return ptr;
}


TClntMsg::TClntMsg(int iface,
		   SPtr<TIPv6Addr> addr, int msgType)
		   :TMsg(iface,addr,msgType)
{
    setDefaults();
}

TClntMsg::~TClntMsg() {
    if (pkt)
	delete [] pkt;
    pkt = 0;
}

void TClntMsg::setDefaults()
{
    FirstTimeStamp = now();
    LastTimeStamp  = now();

    RC  = 0;
    RT  = 0;
    IRT = 0;
    MRT = 0;
    MRC = 0;
    MRD = 0;

#ifndef MOD_DISABLE_AUTH
    DigestType = ClntCfgMgr().getDigest();
    AuthKeys = ClntCfgMgr().AuthKeys;
#endif
    KeyGenNonce = NULL;
    KeyGenNonceLen = 0;
}

unsigned long TClntMsg::getTimeout()
{
    long diff = (LastTimeStamp+RT) - now();
    return (diff<0) ? 0 : diff;
}

void TClntMsg::send()
{
    if (!pkt)
	pkt = new char[getSize()];

    srand(now());
    if (!RC)
	RT=(int)(0.5+IRT+IRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));
    else
	RT =(int)(0.5+2.0*RT+RT*(0.2*(double)rand()/(double)RAND_MAX-0.1));

    if (MRT != 0 && RT>MRT)
	RT = (int)(0.5+MRT + MRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));

    if ((MRD != 0) && (RT>MRD))
	RT = MRD;
    if (MRD) MRD-=RT;

    RC++;

    this->storeSelf(this->pkt);

    SPtr<TIfaceIface> ptrIface = ClntIfaceMgr().getIfaceByID(Iface);
    if (PeerAddr) {
	Log(Debug) << "Sending " << this->getName() << "(opts:";
	SPtr<TOpt> opt;
	firstOption();
	while (opt=getOption()) {
	    Log(Cont) << opt->getOptType() << " ";
	}
	Log(Cont) << ") on " << ptrIface->getName()
		   << "/" << Iface << " to unicast addr " << *PeerAddr << "." << LogEnd;
	ClntIfaceMgr().sendUnicast(Iface,pkt,getSize(),PeerAddr);
    } else {
	Log(Debug) << "Sending " << this->getName() << "(opts:";
	SPtr<TOpt> opt;
	firstOption();
	while (opt=getOption()) {
	    Log(Cont) << opt->getOptType() << " ";
	}
	Log(Cont) << ") on " << ptrIface->getName()
		   << "/" << Iface << " to multicast." << LogEnd;
	ClntIfaceMgr().sendMulticast(Iface, pkt, getSize());
    }
    LastTimeStamp = now();
}

void TClntMsg::copyAAASPI(SPtr<TClntMsg> q) {
    AAASPI = q->getAAASPI();
    SPI = q->getSPI();
    AuthInfoKey = q->getAuthInfoKey();
}

void TClntMsg::setIface(int iface) {
    this->Iface = iface;
    SPtr<TOpt> opt;
    firstOption();
    while ( opt = getOption() ) {
	switch ( opt->getOptType() ) {
	case OPTION_IA_NA: {
	    SPtr<TClntOptIA_NA> ia = (Ptr*) opt;
	    ia->setIface(iface);
		break;
	}
	case OPTION_IA_TA: {
	    SPtr<TClntOptTA> ta = (Ptr*) opt;
	    ta->setIface(iface);
	    break;
	}
	default:
	    continue;
	}
    }
}

/**
 * this function appends authentication option
 *
 * @param AddrMgr pointer to Address Manager
 *
 */
void TClntMsg::appendAuthenticationOption()
{
#ifndef MOD_DISABLE_AUTH
    if (!ClntCfgMgr().getAuthEnabled() || ClntCfgMgr().getDigest() == DIGEST_NONE) {
	Log(Debug) << "Authentication is disabled, not including auth options in message." << LogEnd;
	DigestType = DIGEST_NONE;
	return;
    }

    this->DigestType = ClntCfgMgr().getDigest();

    if (!getOption(OPTION_AUTH)) {
	ClntAddrMgr().firstClient();
	SPtr<TAddrClient> client = ClntAddrMgr().getClient();
	if (client && client->getSPI())
	    this->setSPI(client->getSPI());
	if (client)
	    this->ReplayDetection = client->getNextReplayDetectionSent();
	else
	    this->ReplayDetection = 1;
	Options.push_back(new TClntOptAuthentication(this));
	if (client)
	    client->setSPI(this->getSPI());
    }
#endif
}

void TClntMsg::appendElapsedOption() {
    // include ELAPSED option

    if (!getOption(OPTION_ELAPSED_TIME))
	Options.push_back(new TClntOptElapsed(this));
}

/* CHANGED in this function: According to RFC3315,'status==STATE_NOTCONFIGURED' is not a must.
 * this method adds requested (which have status==STATE_NOTCONFIGURED) options
 */
void TClntMsg::appendRequestedOptions() {

    // find configuration specified in config file
    SPtr<TClntCfgIface> iface = ClntCfgMgr().getIface(this->Iface);
    if (!iface) {
	Log(Error) << "Unable to find interface with ifindex=" << this->Iface << LogEnd;
	return;
    }


    if ( (MsgType==SOLICIT_MSG || MsgType==REQUEST_MSG) &&
	 ClntCfgMgr().getReconfigure())
    {
	SPtr<TOptEmpty> optReconfigure = new TOptEmpty(OPTION_RECONF_ACCEPT, this);
	Options.push_back( (Ptr*) optReconfigure);
    }

    SPtr<TClntOptOptionRequest> optORO = new TClntOptOptionRequest(iface, this);

    if (iface->getUnicast()) {
	optORO->addOption(OPTION_UNICAST);
	Log(Debug) << "Adding UNICAST to ORO." << LogEnd;
    }

    if (ClntCfgMgr().addInfRefreshTime()) {
	optORO->addOption(OPTION_INFORMATION_REFRESH_TIME);
	Log(Debug) << "Adding INFORMATION REFRESH TIME to ORO." << LogEnd;
    }

    // --- option: DNS-SERVERS ---
    if ( iface->isReqDNSServer() ) {
	optORO->addOption(OPTION_DNS_SERVERS);

	List(TIPv6Addr) * dnsLst = iface->getProposedDNSServerLst();
	if (dnsLst->count()) {
	    // if there are any hints specified in config file, include them
	    SPtr<TClntOptDNSServers> opt = new TClntOptDNSServers(dnsLst,this);
	    Options.push_back( (Ptr*)opt );
	}
	iface->setDNSServerState(STATE_INPROCESS);
    }

    // --- option: DOMAINS --
    if ( iface->isReqDomain() ) {
	optORO->addOption(OPTION_DOMAIN_LIST);

	List(string) * domainsLst = iface->getProposedDomainLst();
	if ( domainsLst->count() ) {
	    // if there are any hints specified in config file, include them
	    SPtr<TClntOptDomainName> opt = new TClntOptDomainName(domainsLst,this);
	    Options.push_back( (Ptr*)opt );
	}
	iface->setDomainState(STATE_INPROCESS);
    }

    // --- option: NTP SERVER ---
    if ( iface->isReqNTPServer() ) {
	optORO->addOption(OPTION_SNTP_SERVERS);

	List(TIPv6Addr) * ntpLst = iface->getProposedNTPServerLst();
	if (ntpLst->count()) {
	    // if there are any hints specified in config file, include them
	    SPtr<TClntOptNTPServers> opt = new TClntOptNTPServers(ntpLst,this);
	    Options.push_back( (Ptr*)opt );
	}
	iface->setNTPServerState(STATE_INPROCESS);
    }

    // --- option: TIMEZONE ---
    if ( iface->isReqTimezone() ) {
	optORO->addOption(OPTION_NEW_TZDB_TIMEZONE);

	string timezone = iface->getProposedTimezone();
	if (timezone.length()) {
	    // if there are any hints specified in config file, include them
	    SPtr<TClntOptTimeZone> opt = new TClntOptTimeZone(timezone,this);
	    Options.push_back( (Ptr*)opt );
	}
	iface->setTimezoneState(STATE_INPROCESS);
    }

    // --- option: SIP-SERVERS ---
    if ( iface->isReqSIPServer() ) {
	optORO->addOption(OPTION_SIP_SERVER_A);

	List(TIPv6Addr) * lst = iface->getProposedSIPServerLst();
	if ( lst->count()) {
	    // if there are any hints specified in config file, include them
	    SPtr<TClntOptSIPServers> opt = new TClntOptSIPServers( lst, this );
	    Options.push_back( (Ptr*)opt );
	}
	iface->setSIPServerState(STATE_INPROCESS);
    }

    // --- option: SIP-DOMAINS ---
    if ( iface->isReqSIPDomain() ) {
	optORO->addOption(OPTION_SIP_SERVER_D);

	List(string) * domainsLst = iface->getProposedSIPDomainLst();
	if ( domainsLst->count() ) {
	    // if there are any hints specified in config file, include them
	    SPtr<TClntOptSIPDomain> opt = new TClntOptSIPDomain( domainsLst,this );
	    Options.push_back( (Ptr*)opt );
	}
	iface->setSIPDomainState(STATE_INPROCESS);
    }

    // --- option: FQDN ---
    if ( iface->isReqFQDN() ) {
	optORO->addOption(OPTION_FQDN);

	string fqdn = iface->getProposedFQDN();
	{
	    SPtr<TClntOptFQDN> opt = new TClntOptFQDN( fqdn,this );
	    opt->setSFlag(ClntCfgMgr().getFQDNFlagS());
	    Options.push_back( (Ptr*)opt );
	}
	iface->setFQDNState(STATE_INPROCESS);
    }

    // --- option: NIS-SERVERS ---
    if ( iface->isReqNISServer() ) {
	optORO->addOption(OPTION_NIS_SERVERS);

	List(TIPv6Addr) * lst = iface->getProposedNISServerLst();
	if ( lst->count() ) {
	    // if there are any hints specified in config file, include them
	    SPtr<TClntOptNISServers> opt = new TClntOptNISServers( lst,this );
	    Options.push_back( (Ptr*)opt );
	}
	iface->setNISServerState(STATE_INPROCESS);
    }

    // --- option: NIS-DOMAIN ---
    if ( iface->isReqNISDomain() ) {
	optORO->addOption(OPTION_NIS_DOMAIN_NAME);
	string domain = iface->getProposedNISDomain();
	if (domain.length()) {
	    SPtr<TClntOptNISDomain> opt = new TClntOptNISDomain( domain,this );
	    Options.push_back( (Ptr*)opt );
	}
	iface->setNISDomainState(STATE_INPROCESS);
    }

    // --- option: NIS+-SERVERS ---
    if ( iface->isReqNISPServer() ) {
	optORO->addOption(OPTION_NISP_SERVERS);

	List(TIPv6Addr) * lst = iface->getProposedNISPServerLst();
	if ( lst->count() ) {
	    // if there are any hints specified in config file, include them
	    SPtr<TClntOptNISPServers> opt = new TClntOptNISPServers( lst,this );
	    Options.push_back( (Ptr*)opt );
	}
	iface->setNISPServerState(STATE_INPROCESS);
    }

    // --- option: NIS+-DOMAIN ---
    if ( iface->isReqNISPDomain() ) {
	optORO->addOption(OPTION_NISP_DOMAIN_NAME);
	string domain = iface->getProposedNISPDomain();
	if (domain.length()) {
	    SPtr<TClntOptNISPDomain> opt = new TClntOptNISPDomain( domain,this );
	    Options.push_back( (Ptr*)opt );
	}
	iface->setNISPDomainState(STATE_INPROCESS);
    }

    // --- option: Prefix Delegation ---
    // prefix delegation is supported in a similar way to IA and TA
    // see ClntTransMgr::checkSolicit() for details

    // --- option: LIFETIME ---
    if ( iface->isReqLifetime() && (this->MsgType == INFORMATION_REQUEST_MSG) && optORO->count() )
	optORO->addOption(OPTION_INFORMATION_REFRESH_TIME);

    // --- option: VENDOR-SPEC ---
    if ( iface->isReqVendorSpec() ) {
	optORO->addOption(OPTION_VENDOR_OPTS);
	iface->setVendorSpecState(STATE_INPROCESS);

	SPtr<TOptVendorSpecInfo> optVendor;
	iface->firstVendorSpec();

	while (optVendor = iface->getVendorSpec()) {
	    Options.push_back( (Ptr*) optVendor);
	}
    }

    // --- option: ADDRPARAMS ---
    SPtr<TClntCfgIA> ia;
    iface->firstIA();
    bool addrParams = false;
    while (ia = iface->getIA()) {
	if (ia->getAddrParams())
	    addrParams = true;
    }
    if (addrParams)
	optORO->addOption(OPTION_ADDRPARAMS);

    appendElapsedOption();

    // --- generic options ---
    TClntCfgIface::TOptionStatusLst& genericOpts = iface->getExtraOptions();
    for (TClntCfgIface::TOptionStatusLst::iterator gen = genericOpts.begin();
	 gen!=genericOpts.end();
	 ++gen) {
	if ( (*gen)->State == STATE_NOTCONFIGURED ||
	     (*gen)->State == STATE_INPROCESS ||
	     (*gen)->State == STATE_CONFIRMME) {
	    optORO->addOption( (*gen)->OptionType);
	    if ( (*gen)->Option && (*gen)->Always)
		Options.push_back( (*gen)->Option );
	}
    }

#ifndef MOD_DISABLE_AUTH
    if (this->MsgType == SOLICIT_MSG) {
	    if (ClntCfgMgr().getAuthEnabled()) {
		    // --- option: AAAAUTH ---
		    Options.push_back(new TClntOptAAAAuthentication(this));

		    // request KeyGeneration
		    optORO->addOption(OPTION_KEYGEN);
		    // request Authentication
		    optORO->addOption(OPTION_AUTH);
	    }
    } else {
	/*
	    // --- option: AUTH ---
	    if (ClntCfgMgr().getAuthEnabled() && ClntCfgMgr().getDigest()!=DIGEST_NONE) {
		    Log(Debug) << "Authentication enabled, adding AUTH option." << LogEnd;
		    ClntAddrMgr->firstClient();
		    SPtr<TAddrClient> client = ClntAddrMgr->getClient();
		    if (client && client->getSPI())
			this->setSPI(client->getSPI());
		    Options.push_back(new TClntOptAuthentication(this));
		    client->setSPI(this->getSPI());
	    }
	    */
    }
#endif

#ifdef MOD_REMOTE_AUTOCONF
    if (ClntCfgMgr().getRemoteAutoconf())
	optORO->addOption(OPTION_NEIGHBORS);
#endif

    // final setup: Did we add any options at all?
    if ( optORO->count() )
	Options.push_back( (Ptr*) optORO );
}

/**
 * append all TA options, which are currently in the STATE_NOTCONFIGURED state.
 *
 * @param switchToInProcess - switch them to STATE_INPROCESS state?
 */
void TClntMsg::appendTAOptions(bool switchToInProcess)
{
    SPtr<TClntCfgIface> ptrIface;
    SPtr<TClntCfgTA> ptrTA;
    ClntCfgMgr().firstIface();
    // for each interface...
    while ( ptrIface = ClntCfgMgr().getIface() ) {
	ptrIface->firstTA();
	// ... find TA...
	while ( ptrTA = ptrIface->getTA() ) {
	    if (ptrTA->getState()!=STATE_NOTCONFIGURED)
		continue;
	    // ... which are not yet configured
	    SPtr<TOpt> ptrOpt = new TClntOptTA(ptrTA->getIAID(), this);

	    Options.push_back ( (Ptr*) ptrOpt);
	    Log(Debug) << "TA option (IAID=" << ptrTA->getIAID() << ") was added." << LogEnd;
	    if (switchToInProcess)
		ptrTA->setState(STATE_INPROCESS);
	}
    }

}

bool TClntMsg::appendClientID()
{
    SPtr<TOpt> ptr;
    ptr = new TOptDUID(OPTION_CLIENTID, ClntCfgMgr().getDUID(), this );
    Options.push_back( ptr );
    return true;
}

bool TClntMsg::check(bool clntIDmandatory, bool srvIDmandatory) {
    bool status = TMsg::check(clntIDmandatory, srvIDmandatory);

    SPtr<TOptDUID> clnID;

    if ( (clnID=(Ptr*)getOption(OPTION_CLIENTID)) &&
	 !( *(clnID->getDUID())==(*(ClntCfgMgr().getDUID())) ) ) {
	Log(Warning) << "Message " << this->getName()
	    << " received with mismatched ClientID option. Message dropped." << LogEnd;
	return false;
    }

    return status;
}

/**
 * method is used to process received REPLY message (as an answer for REQUEST or
 * as an answer for SOLICIT with RAPID COMMIT option)
 *
 * @param reply
 */

void TClntMsg::answer(SPtr<TClntMsg> reply)
{
    SPtr<TOptDUID> ptrDUID;
    ptrDUID = (Ptr*) reply->getOption(OPTION_SERVERID);
    if (!ptrDUID) {
      Log(Warning) << "Received REPLY message without SERVER ID option. Message ignored." << LogEnd;
      return;
    }

    // analyse all options received
    SPtr<TOpt> option;

    // find ORO in received options
    reply->firstOption();
    SPtr<TClntOptOptionRequest> optORO = (Ptr*) this->getOption(OPTION_ORO);

    reply->firstOption();
    while (option = reply->getOption() ) {
	switch (option->getOptType())
	{

	case OPTION_IA_NA:
	{
	    SPtr<TClntOptIA_NA> clntOpt = (Ptr*)option;
	    if (clntOpt->getStatusCode()!=STATUSCODE_SUCCESS) {
		Log(Warning) << "Received IA (IAID=" << clntOpt->getIAID() << ") with non-success status:"
			     << clntOpt->getStatusCode() << ", IA ignored." << LogEnd;
		break;
	    }

	    // configure received IA
	    clntOpt->setContext(ptrDUID->getDUID(), 0/* srvAddr used is unicast */, this->Iface);
	    clntOpt->doDuties();

	    // delete that IA from request list
	    for (TOptList::iterator requestOpt = Options.begin(); requestOpt!=Options.end(); ++requestOpt)
	    {
		if ( (*requestOpt)->getOptType()!=OPTION_IA_NA)
		    continue;
		SPtr<TClntOptIA_NA> ptrIA = (Ptr*) (*requestOpt);
		if ( ptrIA->getIAID() == clntOpt->getIAID() )
		{
		    requestOpt = Options.erase(requestOpt);
		    break;
		}
	    }

	    // delete request for IA, if it was mentioned in Option Request
	    if ( optORO && optORO->isOption(OPTION_IA_NA) )
		optORO->delOption(OPTION_IA_NA);

	    break;
	}
	case OPTION_IA_TA:
	{
	    SPtr<TClntOptTA> ta = (Ptr*) option;
	    if (ta->getStatusCode()!=STATUSCODE_SUCCESS) {
		Log(Warning) << "Received TA (IAID=" << ta->getIAID() << ") with non-success status:"
			     << ta->getStatusCode() << ", TA ignored." << LogEnd;
		break;
	    }

	    SPtr<TOpt> requestOpt;

	    // delete that TA from request list
	    for (TOptList::iterator requestOpt = Options.begin(); requestOpt!=Options.end(); ++requestOpt)
	    {
		if ( (*requestOpt)->getOptType()!=OPTION_IA_TA)
		    continue;
		SPtr<TClntOptTA> ptrTA = (Ptr*) (*requestOpt);
		if ( ta->getIAID() == ptrTA->getIAID() )
		{
		    requestOpt = Options.erase(requestOpt);
		    break;
		}
	    }

	    ta->setIface(Iface);
	    ta->doDuties();
	    break;
	}

	case OPTION_IA_PD:
	{
	    SPtr<TClntOptIA_PD> pd = (Ptr*) option;

	    if (pd->getStatusCode()!=STATUSCODE_SUCCESS) {
		Log(Warning) << "Received PD (PDAID=" << pd->getIAID() << ") with non-success status:"
			     << pd->getStatusCode() << ", PD ignored." << LogEnd;
		break;
	    }

	    // configure received PD
	    pd->setContext(ptrDUID->getDUID(), 0/* srvAddr used in unicast */, this);
	    pd->doDuties();

	    // delete that PD from request list
	    for (TOptList::iterator requestOpt = Options.begin(); requestOpt!=Options.end(); ++requestOpt)
	    {
		if ( (*requestOpt)->getOptType()!=OPTION_IA_PD)
		    continue;
		SPtr<TClntOptIA_PD> reqPD = (Ptr*) (*requestOpt);
		if ( pd->getIAID() == reqPD->getIAID() )
		{
		    requestOpt = Options.erase(requestOpt);
		    break;
		}
	    }

	    // delete request for PD, if it was mentioned in Option Request
	    if ( optORO && optORO->isOption(OPTION_IA_PD) )
		optORO->delOption(OPTION_IA_PD);

	    break;
	    }
#ifdef MOD_REMOTE_AUTOCONF
	case OPTION_NEIGHBORS:
	  {
	    SPtr<TOptAddrLst> neighbors = (Ptr*) option;
	    ClntTransMgr().updateNeighbors(reply->getIface(), neighbors);
	    break;
	  }
#endif

	case OPTION_IAADDR:
	    Log(Warning) << "Option OPTION_IAADDR misplaced." << LogEnd;
	    break;

	default:
	{

	    SPtr<TOpt> requestOpt;
	    if ( optORO && (optORO->isOption(option->getOptType())) )
		optORO->delOption(option->getOptType());

	    //Log(Debug) << "Setting up option " << option->getOptType() << "." << LogEnd;
	    if (!option->doDuties()) {
		Log(Warning) << "Setting option " << option->getOptType() << " failed." << LogEnd;
		// do nothing about it
	    }


	    // find options specified in this message
	    firstOption();
	    while ( requestOpt = getOption() ) {
		if ( requestOpt->getOptType() == option->getOptType() )
		{
		    delOption(requestOpt->getOptType());
		}//if
	    }//while
	}
	} // switch
    }
    //Options and IAs serviced by server are removed from requestOptions list

    SPtr<TOpt> requestOpt;
    firstOption();
    bool iaLeft = false;
    bool taLeft = false;
    bool pdLeft = false;
    while ( requestOpt = getOption() ) {
	if (requestOpt->getOptType() == OPTION_IA_NA) iaLeft = true;
	if (requestOpt->getOptType() == OPTION_IA_TA) taLeft = true;
	if (requestOpt->getOptType() == OPTION_IA_PD) pdLeft = true;
    }

//    taLeft = false;

    if (iaLeft || taLeft || pdLeft) {
	// send new Request to another server
	Log(Notice) << "There are still " << (iaLeft?"some IA(s)":"")
		    << (taLeft?"TA":"") << (pdLeft?"some PD(s)":"") << " to configure." << LogEnd;
	ClntTransMgr().sendRequest(this->Options, this->Iface);
    } else {
	if (optORO)
	    optORO->delOption(OPTION_ADDRPARAMS); // don't insist on getting ADDR-PARAMS

	if ( optORO && (optORO->count()) )
	{
	    Log(Warning) << "All IA(s), TA and PD(s) has been configured, but some options (";
	    for (int i=0; i< optORO->count(); i++)
		Log(Cont) << optORO->getReqOpt(i) << " ";
	    Log(Cont) << ") were not assigned." << LogEnd;

	    if (ClntCfgMgr().insistMode()) {
		Log(Notice) << "Insist-mode enabled, sending INF-REQUEST." << LogEnd;
		ClntTransMgr().sendInfRequest(this->Options, this->Iface);
	    } else {
		Log(Notice) << "Insist-mode disabled, giving up (not sending INF-REQUEST)." << LogEnd;
		/// @todo: set proper options to FAILED state
	    }
	    }
    }
    IsDone = true;
    return;
}

bool TClntMsg::validateReplayDetection() {
    if (this->MsgType == SOLICIT_MSG)
	return true;

    ClntAddrMgr().firstClient();
    SPtr<TAddrClient> client = ClntAddrMgr().getClient();

    if (!client) {
	Log(Crit) << "Something is wrong, VERY wrong. Info about this client (myself) is not found." << LogEnd;
	return false;
    }

    if (!client->getReplayDetectionRcvd() && !this->ReplayDetection)
	return true;

    if (client->getReplayDetectionRcvd() < this->ReplayDetection) {
	Log(Debug)
	    << "Replay detection field should be greater than "
	    << client->getReplayDetectionRcvd()
	    << " and it actually is "
	    << this->ReplayDetection << LogEnd;
	client->setReplayDetectionRcvd(this->ReplayDetection);
	return true;
    } else {
	Log(Warning) << "Replayed message detected: Replay detection field should be greater than "
		     << client->getReplayDetectionRcvd()
		     << ", but "
		     << this->ReplayDetection
		     << " received." << LogEnd;
	return false;
    }
}
