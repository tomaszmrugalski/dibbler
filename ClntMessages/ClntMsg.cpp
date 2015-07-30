/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *          Mateusz Ozga <matozga@gmail.com>
 * 
 * released under GNU GPL v2 only licence
 */

#include <cmath>
#include <sstream>
#include <stdlib.h>
#include "Portable.h"
#include "ClntCfgMgr.h"

#include "ClntMsg.h"
#include "ClntTransMgr.h"

#include "OptGeneric.h"
#include "OptEmpty.h"
#include "OptAddrLst.h"
#include "OptAddr.h"
#include "OptDUID.h"
#include "OptDomainLst.h"
#include "OptRtPrefix.h"
#include "ClntOptIA_NA.h"
#include "ClntOptIA_PD.h"
#include "ClntOptTA.h"
#include "ClntOptOptionRequest.h"
#include "ClntOptPreference.h"
#include "OptReconfigureMsg.h"
#include "ClntOptElapsed.h"
#include "ClntOptStatusCode.h"
#include "ClntOptTimeZone.h"
#include "ClntOptFQDN.h"
#include "OptAddrLst.h"
#include "ClntOptLifetime.h"
#include "OptAuthentication.h"
#include "hex.h"

#include "Logger.h"

using namespace std;

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
	unsigned short code   = readUint16(buf+pos);
	pos += sizeof(uint16_t);
	unsigned short length = readUint16(buf+pos);
	pos += sizeof(uint16_t);
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
	ptr.reset();

	switch (code) {
	case OPTION_CLIENTID:
	case OPTION_SERVERID:
	    ptr = new TOptDUID(code, buf+pos, length, this);
	    break;
	case OPTION_IA_NA:
	    ptr = new TClntOptIA_NA(buf+pos, length, this);
	    break;
	case OPTION_IA_PD:
	    ptr = new TClntOptIA_PD(buf+pos, length, this);
	    break;
	case OPTION_ORO:
	    ptr = new TClntOptOptionRequest(buf+pos, length, this);
	    break;
	case OPTION_PREFERENCE:
	    ptr = new TClntOptPreference(buf+pos, length, this);
	    break;
	case OPTION_ELAPSED_TIME:
	    ptr = new TClntOptElapsed(buf+pos, length, this);
	    break;
	case OPTION_UNICAST:
	    ptr = new TOptAddr(OPTION_UNICAST, buf+pos, length, this);
	    break;
	case OPTION_STATUS_CODE:
	    ptr = new TClntOptStatusCode(buf+pos, length, this);
	    break;
	case OPTION_RAPID_COMMIT:
	    ptr = new TOptEmpty(code, buf+pos, length, this);
	    break;
	case OPTION_NIS_SERVERS:
	case OPTION_NISP_SERVERS:
	case OPTION_DNS_SERVERS:
	case OPTION_SNTP_SERVERS:
	case OPTION_SIP_SERVER_A:
	    ptr = new TOptAddrLst(code, buf+pos, length, this);
	    break;
	case OPTION_DOMAIN_LIST:
	case OPTION_SIP_SERVER_D:
	case OPTION_NIS_DOMAIN_NAME:
	case OPTION_NISP_DOMAIN_NAME:
	    ptr = new TOptDomainLst(code, buf+pos, length, this);
	    break;
	case OPTION_NEW_TZDB_TIMEZONE:
	    ptr = new TClntOptTimeZone(buf+pos, length, this);
	    break;
	case OPTION_FQDN:
	    ptr = new TClntOptFQDN(buf+pos, length, this);
	    break;
	case OPTION_INFORMATION_REFRESH_TIME:
	    ptr = new TClntOptLifetime(buf+pos, length, this);
	    break;
	case OPTION_AFTR_NAME:
	    ptr = new TOptDomainLst(code, buf+pos, length, this);
	    break;
	case OPTION_IA_TA: {
	    ptr = new TClntOptTA(buf+pos, length, this);
	    break;
	}
#ifndef MOD_DISABLE_AUTH
	case OPTION_AUTH:
	    if (ClntCfgMgr().getAuthProtocol() == AUTH_PROTO_DIBBLER) {
		DigestType_ = ClntCfgMgr().getDigest();
	    }
            ptr = new TOptAuthentication(buf+pos, length, this);
	    break;
#endif
	case OPTION_VENDOR_OPTS: {
	    ptr = new TOptVendorSpecInfo(code, buf+pos, length, this);
	    break;
	}
	case OPTION_RECONF_MSG: {
	    ptr = new TOptReconfigureMsg(buf+pos, length, this);
	    break;
	}
	case OPTION_RECONF_ACCEPT: {
            ptr = new TOptEmpty(OPTION_RECONF_ACCEPT, buf+pos, length, this);
            break;
        }
        case OPTION_NEXT_HOP: {
            ptr = new TOptAddr(code, buf+pos, length, this);
            break;
        }
        case OPTION_RTPREFIX: {
            ptr = new TOptRtPrefix(buf+pos, length, this);
            break;
        }
	case OPTION_USER_CLASS:
	case OPTION_VENDOR_CLASS:
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

    // @todo: confirm verification here

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
	    ptr = new TOptDomainLst(code, buf, length, this);
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
}

void TClntMsg::setDefaults()
{
    FirstTimeStamp = time(NULL);
    LastTimeStamp  = time(NULL);

    RC  = 0;
    RT  = 0;
    IRT = 0;
    MRT = 0;
    MRC = 0;
    MRD = 0;

#ifndef MOD_DISABLE_AUTH
    DigestType_ = ClntCfgMgr().getDigest();
#endif

    /// @todo: This should be moved to TMsg
    PeerAddr_.reset();
}

unsigned long TClntMsg::getTimeout()
{
    long diff = (LastTimeStamp+RT) - time(NULL);
    return (diff<0) ? 0 : diff;
}

void TClntMsg::send()
{
    char* pkt = new char[getSize()];

    srand((uint32_t)time(NULL));
    if (!RC)
	RT=(int)(0.5+IRT+IRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));
    else
	RT =(int)(0.5+2.0*RT+RT*(0.2*(double)rand()/(double)RAND_MAX-0.1));

    if (MRT != 0 && RT>MRT)
	RT = (int)(0.5+MRT + MRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));

    if ((MRD != 0) && (RT>MRD))
	RT = MRD;
    if (MRD) 
      MRD -= RT;

    RC++;

    this->storeSelf(pkt);

    SPtr<TIfaceIface> ptrIface = ClntIfaceMgr().getIfaceByID(Iface);
    if (!ptrIface) {
        Log(Error) << "Unable to find interface with ifindex=" << Iface
                   << ". Message not sent." << LogEnd;
        delete [] pkt;
        return;
    }
    if (PeerAddr_) {
	Log(Debug) << "Sending " << this->getName() << "(opts:";
	SPtr<TOpt> opt;
	firstOption();
	while (opt=getOption()) {
	    Log(Cont) << opt->getOptType() << " ";
	}
	Log(Cont) << ") on " << ptrIface->getName()
		   << "/" << Iface << " to unicast addr " << *PeerAddr_ << "." << LogEnd;
	ClntIfaceMgr().sendUnicast(Iface,pkt,getSize(),PeerAddr_);
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
    LastTimeStamp = time(NULL);
    delete [] pkt;
}

void TClntMsg::copyAAASPI(SPtr<TClntMsg> q) {
    SPI_ = q->SPI_;
    AuthKey_ = q->AuthKey_;
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
 * @brief appends authentication option.
 *
 */
void TClntMsg::appendAuthenticationOption()
{
#ifndef MOD_DISABLE_AUTH
    uint8_t algorithm = 0; // algorithm is protocol specific

    DigestType_ = DIGEST_NONE;

    // ClntAddrMgr().firstClient();
    // SPtr<TAddrClient> client = ClntAddrMgr().getClient();
    string realm;

    switch (ClntCfgMgr().getAuthProtocol()) {
    case AUTH_PROTO_NONE: {
        algorithm = 0;

        // Do not sent AUTH with proto=0.
        return;
    }
    case AUTH_PROTO_DELAYED: {
        algorithm = 1;
        realm = ClntCfgMgr().getAuthRealm();
        break;
    }
    case AUTH_PROTO_RECONFIGURE_KEY: { // RFC 3315, section 21.5.1
        // When reconfigure-key is enabled, client does not send anything
        return;
    }
    case AUTH_PROTO_DIBBLER: { // Mechanism proposed by Kowalczuk
        DigestType_ = ClntCfgMgr().getDigest();
        algorithm = static_cast<uint8_t>(DigestType_);
        setSPI(ClntCfgMgr().getSPI());

        SPtr<TClntOptOptionRequest> optORO = (Ptr*) getOption(OPTION_ORO);

        if (optORO) {
          // request Authentication
          optORO->addOption(OPTION_AUTH);
        }

        break;
    }
    default: {
        Log(Error) << "Auth: Invalid protocol specified. Can't sent AUTH option." << LogEnd;
        return;
    }
    }

    SPtr<TOptAuthentication> auth =
        new TOptAuthentication(ClntCfgMgr().getAuthProtocol(),
                               algorithm,
                               ClntCfgMgr().getAuthReplay(), this);

    // Realm is used by delayed-auth only. Even fro delayed-auth, it is set
    // only for non-SOLICIT messages
    if (MsgType != SOLICIT_MSG && !realm.empty()) {
        auth->setRealm(realm);
    }

    // replay detection
    if (ClntCfgMgr().getAuthReplay() == AUTH_REPLAY_MONOTONIC) {
        auth->setReplayDetection(ClntAddrMgr().getNextReplayDetectionValue());
    }

    addOption((Ptr*)auth);

    // otherwise replay value is zero
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
	    Options.push_back( new TOptAddrLst(OPTION_DNS_SERVERS, *dnsLst, this) );
	}
	iface->setDNSServerState(STATE_INPROCESS);
    }

    // --- option: DOMAINS --
    if ( iface->isReqDomain() ) {
	optORO->addOption(OPTION_DOMAIN_LIST);

	List(string) * domainsLst = iface->getProposedDomainLst();
	if ( domainsLst->count() ) {
	    // if there are any hints specified in config file, include them
	    Options.push_back( new TOptDomainLst(OPTION_DOMAIN_LIST, *domainsLst, this));
	}
	iface->setDomainState(STATE_INPROCESS);
    }

    // --- option: NTP SERVER ---
    if ( iface->isReqNTPServer() ) {
	optORO->addOption(OPTION_SNTP_SERVERS);

	List(TIPv6Addr) * ntpLst = iface->getProposedNTPServerLst();
	if (ntpLst->count()) {
	    // if there are any hints specified in config file, include them
	    Options.push_back( new TOptAddrLst(OPTION_SNTP_SERVERS, *ntpLst, this) );
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
	    Options.push_back( new TOptAddrLst(OPTION_SIP_SERVER_A, *lst, this ) );
	}
	iface->setSIPServerState(STATE_INPROCESS);
    }

    // --- option: SIP-DOMAINS ---
    if ( iface->isReqSIPDomain() ) {
	optORO->addOption(OPTION_SIP_SERVER_D);

	List(string) * domainsLst = iface->getProposedSIPDomainLst();
	if ( domainsLst->count() ) {
	    // if there are any hints specified in config file, include them
	    Options.push_back( new TOptDomainLst(OPTION_SIP_SERVER_D, *domainsLst, this ));
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
	    Options.push_back( new TOptAddrLst(OPTION_NIS_SERVERS, *lst, this ));
	}
	iface->setNISServerState(STATE_INPROCESS);
    }

    // --- option: NIS-DOMAIN ---
    if ( iface->isReqNISDomain() ) {
	optORO->addOption(OPTION_NIS_DOMAIN_NAME);
	string domain = iface->getProposedNISDomain();
	if (domain.length()) {
	    Options.push_back( new TOptDomainLst(OPTION_NIS_DOMAIN_NAME, domain, this) );
	}
	iface->setNISDomainState(STATE_INPROCESS);
    }

    // --- option: NIS+-SERVERS ---
    if ( iface->isReqNISPServer() ) {
	optORO->addOption(OPTION_NISP_SERVERS);

	List(TIPv6Addr) * lst = iface->getProposedNISPServerLst();
	if ( lst->count() ) {
	    // if there are any hints specified in config file, include them
	    Options.push_back( new TOptAddrLst(OPTION_NISP_SERVERS, *lst, this) );
	}
	iface->setNISPServerState(STATE_INPROCESS);
    }

    // --- option: NIS+-DOMAIN ---
    if ( iface->isReqNISPDomain() ) {
	optORO->addOption(OPTION_NISP_DOMAIN_NAME);
	string domain = iface->getProposedNISPDomain();
	if (domain.length()) {
	    Options.push_back( new TOptDomainLst(OPTION_NISP_DOMAIN_NAME, domain, this) );
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

    // --- option: Routing ---
    if ( (this->MsgType == SOLICIT_MSG || this->MsgType == REQUEST_MSG ||     
         this->MsgType == RENEW_MSG || this->MsgType ==  REBIND_MSG ||
         this->MsgType == INFORMATION_REQUEST_MSG) &&
         iface->isRoutingEnabled() ) {

        optORO->addOption(OPTION_NEXT_HOP);
        optORO->addOption(OPTION_RTPREFIX);
        
        // only for debugging
        Log(Debug) << "Adding NEXT_HOP and RTPREFIX to ORO." << LogEnd;
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
    SPtr<TDUID> duid = ptrDUID->getDUID();

    SPtr<TClntIfaceIface> iface = (Ptr*)ClntIfaceMgr().getIfaceByID(getIface());
    if (!iface) {
        Log(Error) << "Unable to find physical interface with ifindex=" << getIface() << LogEnd;
        return;
    }

    SPtr<TClntCfgIface> cfgIface = ClntCfgMgr().getIface( getIface() );
    if (!cfgIface) {
        Log(Error) << "Unable to find configuration interface with ifindex=" << getIface() << LogEnd;
    }

    // analyse all options received
    SPtr<TOpt> option;

    // Check authentication first. If the checks fail, we need to drop the whole message
    // without using its contents.
    if (!reply->checkReceivedAuthOption()) {
        Log(Warning) << "AUTH: AUTH option verification failed. Ignoring message." << LogEnd;
        return;
    }

    // find ORO in received options
    SPtr<TClntOptOptionRequest> optORO = (Ptr*) this->getOption(OPTION_ORO);
    
    reply->firstOption();
    while (option = reply->getOption() ) {

	if (optORO)
	  optORO->delOption(option->getOptType()); // delete received option from ORO

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
	    clntOpt->setContext(duid, SPtr<TIPv6Addr>()/* srvAddr used is unicast */, this->Iface);
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

            if (!pd->getOption(OPTION_IAPREFIX)) {
                Log(Notice) << "Received IA_PD without prefixes, ignoring." << LogEnd;
                break;
            }

            bool pdOk = true;
            int prefixCount = pd->countPrefixes();
            pd->firstPrefix();
            SPtr<TClntOptIAPrefix> ppref;
            while (ppref = pd->getPrefix()) {
                if (!ppref->isValid()) {
                    Log(Warning) << "Option IA_PREFIX from IA_PD " <<
                                 pd->getIAID() << " is not valid." << LogEnd;
                    // RFC 3633, section 10:
                    // A requesting router discards any prefixes for which the
                    // preferred lifetime is greater than the valid lifetime.
                    pd->deletePrefix(ppref);
                    prefixCount--;
                    if (!prefixCount) {
                        // ia_pd hasn't got any valid prefixes.
                        if (ClntCfgMgr().insistMode()) {
                            // if insist-mode is enabled and one of received
                            // pd's has no valid prefixes, answer is rejected.
                            pdOk = false;
                        }
                        break;
                    }
                }
            }
            if (!pdOk) {
                break;
            }

	    // configure received PD
	    pd->setContext(duid, SPtr<TIPv6Addr>()/* srvAddr used in unicast */, this);
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
        case OPTION_DNS_SERVERS:
            {
                SPtr<TOptAddrLst> dnsservers = (Ptr*) option;
                cfgIface->setDNSServerState(STATE_CONFIGURED);
                iface->setDNSServerLst(duid, reply->getRemoteAddr(), dnsservers->getAddrLst());
                break;
            }
        case OPTION_NIS_SERVERS:
            {
                SPtr<TOptAddrLst> nisservers = (Ptr*) option;
                cfgIface->setNISServerState(STATE_CONFIGURED);
                iface->setNISServerLst(duid, reply->getRemoteAddr(), nisservers->getAddrLst());
                break;
            }
        case OPTION_NISP_SERVERS:
            {
                SPtr<TOptAddrLst> nispservers = (Ptr*) option;
                cfgIface->setNISPServerState(STATE_CONFIGURED);
                iface->setNISPServerLst(duid, reply->getRemoteAddr(), nispservers->getAddrLst());
                break;
            }
        case OPTION_SNTP_SERVERS:
            {
                SPtr<TOptAddrLst> ntpservers = (Ptr*) option;
                cfgIface->setNTPServerState(STATE_CONFIGURED);
                iface->setNTPServerLst(duid, reply->getRemoteAddr(), ntpservers->getAddrLst());
                break;
            }
        case OPTION_SIP_SERVER_A:
            {
                SPtr<TOptAddrLst> sipservers = (Ptr*) option;
                cfgIface->setSIPServerState(STATE_CONFIGURED);
                iface->setSIPServerLst(duid, reply->getRemoteAddr(), sipservers->getAddrLst());
                break;
            }
        case OPTION_DOMAIN_LIST:
            {
                SPtr<TOptDomainLst> domains = (Ptr*) option;
                cfgIface->setDomainState(STATE_CONFIGURED);
                iface->setDomainLst(duid, reply->getRemoteAddr(), domains->getDomainLst() );
                break;
            }
        case OPTION_SIP_SERVER_D:
            {
                SPtr<TOptDomainLst> sipdomains = (Ptr*) option;
                cfgIface->setSIPDomainState(STATE_CONFIGURED);
                iface->setSIPDomainLst(duid, reply->getRemoteAddr(), sipdomains->getDomainLst() );
                break;
            }
        case OPTION_NIS_DOMAIN_NAME:
            {
                SPtr<TOptDomainLst> nisdomain = (Ptr*) option;
                List(string) domains = nisdomain->getDomainLst();
                if (domains.count() == 1) {
                    cfgIface->setNISDomainState(STATE_CONFIGURED);
                    iface->setNISDomain(duid, reply->getRemoteAddr(), nisdomain->getDomain());
                } else {
                    Log(Warning) << "Malformed NIS Domain option received. " << domains.count()
                                 << " domain(s) received, expected exactly 1." << LogEnd;
                    cfgIface->setNISDomainState(STATE_FAILED);
                }
                break;
            }
        case OPTION_NISP_DOMAIN_NAME:
            {
                SPtr<TOptDomainLst> nispdomain = (Ptr*) option;
                List(string) domains = nispdomain->getDomainLst();
                if (domains.count() == 1) {
                    cfgIface->setNISPDomainState(STATE_CONFIGURED);
                    iface->setNISPDomain(duid, reply->getRemoteAddr(), nispdomain->getDomain());
                } else {
                    Log(Warning) << "Malformed NIS+ Domain option received. " << domains.count()
                                 << " domain(s) received, expected exactly 1." << LogEnd;
                    cfgIface->setNISDomainState(STATE_FAILED);
                }
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

bool TClntMsg::checkReceivedAuthOption() {

#ifdef MOD_DISABLE_AUTH
    return true;
#else

  // Note that this method does not verify digests. That is verified elsewhere
  // see TMsg::validateAuthInfo() from TClntIfaceMgr::select() and from
  // TSrvIfaceMgr::select()
  // This method checks additional things.

    // If replay detection fails, we don't bother to try anything fancy
    if (!validateReplayDetection()) {
        return false;
    }

    switch (ClntCfgMgr().getAuthProtocol()) {
    case AUTH_PROTO_NONE: {
        return true;
    }
    case AUTH_PROTO_DELAYED: {
        SPtr<TOptAuthentication> auth = (Ptr*)getOption(OPTION_AUTH);
        if (!auth) {
            return false;
        }
        if (auth->getProto() != AUTH_PROTO_DELAYED) {
            Log(Warning) << "AUTH: Bad protocol in auth: expected 2(delayed-auth), but got "
                         << int(auth->getProto()) << ", auth option ignored." << LogEnd;
            return false;
        }
        if (auth->getAlgorithm() != 1) {
            Log(Warning) << "AUTH: Bad algorithm in auth option: expected 1 (HMAC-MD5), but got "
                         << int(auth->getAlgorithm()) << ", key ignored." << LogEnd;
            return false;
        }
        if (auth->getRDM() != AUTH_REPLAY_NONE) {
            Log(Warning) << "AUTH: Bad replay detection method (RDM) value: expected 0,"
                         << ", but got " << auth->getRDM() << LogEnd;
            // This is small issue enough, so we can continue.
        }

        return true;
    }
    case AUTH_PROTO_RECONFIGURE_KEY: {

        bool optional = (MsgType != RECONFIGURE_MSG);

        SPtr<TOptAuthentication> auth = (Ptr*)getOption(OPTION_AUTH);
        if (!auth) {
            // there's no auth option. We can't store anything
            return optional;
        }
        if (auth->getProto() != AUTH_PROTO_RECONFIGURE_KEY) {
            Log(Warning) << "AUTH: Bad protocol in auth: expected 3(reconfigure-key), but got "
                         << auth->getProto() << ", key ignored." << LogEnd;
            return optional;
        }
        if (auth->getAlgorithm() != 1) {
            Log(Warning) << "AUTH: Bad algorithm in auth option: expected 1, but got "
                         << auth->getAlgorithm() << ", key ignored." << LogEnd;
            return optional;
        }
        if (auth->getRDM() != AUTH_REPLAY_NONE) {
            Log(Warning) << "AUTH: Bad replay detection method (RDM) value: expected 0,"
                         << ", but got " << auth->getRDM() << LogEnd;
            // This is small issue enough, so we can continue.
        }

        vector<uint8_t> key;
        auth->getPayload(key);

        if (key.size() != RECONFIGURE_KEY_AUTHINFO_SIZE) {
            Log(Warning) << "AUTH: Invalid authentication information length, expected "
                         << RECONFIGURE_KEY_AUTHINFO_SIZE
                         << "(1 for type, 16 for reconfigure-key value), got "
                         << key.size() << LogEnd;
            return optional;
        }

        switch (MsgType) {
        case RECONFIGURE_MSG: {
            /// @todo calculate HMAC-MD5 checksum and compare it against stored key
            Log(Error) << "Support for reconfigure-key in RECONFIGURE message not implementd yet."
                       << LogEnd;
            return false;
        }

        case REPLY_MSG: {
            if (key[0] != 1) { // see RFC3315, section 21.5.1
                Log(Warning) << "AUTH: Invalid type " << key[0] << " in AUTH for reconfigure-key"
                             << " protocol, expected 1, key ingored." << LogEnd;
                return true;
            }

            key.erase(key.begin()); // delete first octect (it is type 1)

            // Store the reconfigure-key
            ClntAddrMgr().firstClient();
            SPtr<TAddrClient> client = ClntAddrMgr().getClient();
            if (!client) {
                Log(Crit) << "Auth: internal error. Info about this client (myself) is not found." << LogEnd;
                return false;
            }
            client->ReconfKey_ = key;
            Log(Info) << "AUTH: Received reconfigure-key " << hexToText(key, true, false)
                      << LogEnd;
            return true;
        }
        default:
            Log(Warning) << "AUTH: AUTH option not expected in message " << MsgType << LogEnd;
            return true;
        }
    } 
    case AUTH_PROTO_DIBBLER: {
      return true;
    }
    }
#endif

    return false;
}

void TClntMsg::getReconfKeyFromAddrMgr() {
    ClntAddrMgr().firstClient();
    SPtr<TAddrClient> client = ClntAddrMgr().getClient();
    if (!client) {
        Log(Crit) << "Auth: internal error. Info about this client (myself) is not found." << LogEnd;
        return;
    }
    AuthKey_ = client->ReconfKey_;
}

bool TClntMsg::validateReplayDetection() {

#ifdef MOD_DISABLE_AUTH
    return true;
#else
    if (ClntCfgMgr().getAuthReplay() == AUTH_REPLAY_NONE) {
        // we don't care about replay detection
        return true;
    }

    // get the client's (ours) information
    ClntAddrMgr().firstClient();
    SPtr<TAddrClient> client = ClntAddrMgr().getClient();
    if (!client) {
	Log(Crit) << "Auth: internal error. Info about this client (myself) is not found." << LogEnd;
	return false;
    }

    SPtr<TOptAuthentication> auth = (Ptr*)getOption(OPTION_AUTH);
    if (!auth) {
        // there's no auth option. We can't protect against replays
        return true;
    }

    uint64_t received = auth->getReplayDetection();
    uint64_t last_received = client->getReplayDetectionRcvd();

    if (last_received < received) {
	Log(Debug) << "Auth: Replay detection field should be greater than "
                   << last_received << " and it actually is ("
                   << received << ")" << LogEnd;
	client->setReplayDetectionRcvd(received);
	return true;
    } else {
	Log(Warning) << "Auth: Replayed message detected: previously received: "
                     << last_received << ", now received " << received << LogEnd;
	return false;
    }

    return true; // not really needed
#endif
}

void TClntMsg::deletePD(SPtr<TOpt> pd_) {
    SPtr<TClntOptIA_PD> pd = (Ptr*) pd_;
    for (TOptList::iterator opt = Options.begin(); opt != Options.end(); ++opt)
    {
        if ( (*opt)->getOptType() != OPTION_IA_PD)
            continue;
        SPtr<TClntOptIA_PD> delPD = (Ptr*) (*opt);
        if ( pd->getIAID() == delPD->getIAID() )
        {
            opt = Options.erase(opt);
            break;
        }
    }
}
