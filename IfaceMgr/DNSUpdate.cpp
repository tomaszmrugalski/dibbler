/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
 *          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
 *          from ENSEEIHT, Toulouse, France
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *          Tomasz Mrugalski <thomson@klub.com.pl>
 * released under GNU GPL v2 licence
 *
 */

#include "DNSUpdate.h"
#include "Portable.h"
#include "Logger.h"
#include <stdio.h>
#include "sha256.h"

using namespace std;

_addr ToPoslibAddr(const std::string& text_addr) {
    _addr dst;
    memset(&dst, 0, sizeof(dst));

    // It seems that Windows now have sockaddr_storage with ss_family field.
	// It used to be sa_family on earlier Windows.
	// dst.sa_family = AF_INET6;
    dst.ss_family = AF_INET6;

	txt_to_addr(&dst, text_addr.c_str());
    return dst;
}

DNSUpdate::DNSUpdate(const std::string& dns_address, const std::string& zonename,
		     const std::string& hostname, std::string hostip,
		     DnsUpdateMode updateMode, DnsUpdateProtocol proto /* = DNSUPDATE_TCP */)
    :Message_(NULL), DnsAddr_(dns_address), Hostip_(hostip), UpdateMode_(updateMode), 
     Proto_(proto), Fudge_(0) {

    if (UpdateMode_ == DNSUPDATE_AAAA || UpdateMode_ == DNSUPDATE_AAAA_CLEANUP) {
	splitHostDomain(hostname);
    } else {
	Hostname_ = hostname;
	Zoneroot_ = domainname(zonename.c_str());
    }


    TTL_ = string(DNSUPDATE_DEFAULT_TTL);
}

DNSUpdate::~DNSUpdate() {
    if (Message_)
	delete Message_;
}

/**
 * splits fqdn (e.g. malcolm.example.com) into hostname (e.g. malcolm) and domain (e.g. example.com)
 *
 * @param fqdnName
 */
void DNSUpdate::splitHostDomain(std::string fqdnName) {
    std::string::size_type dotpos = fqdnName.find(".");
    string hostname = "";
    string domain = "";

    if (dotpos == string::npos) {
	Log(Warning) << "Name provided for DNS update is not a FQDN. [" << fqdnName
		     << "]." << LogEnd;
	Hostname_ = fqdnName;
    }
    else {
	Hostname_ = fqdnName.substr(0, dotpos);
	string domain = fqdnName.substr(dotpos + 1, fqdnName.length() - dotpos - 1);
	Zoneroot_ = domainname(domain.c_str());
    }
}

DnsUpdateResult DNSUpdate::run(int timeout){

    Log(Info) << "DDNS: Performing DNS Update over " << protoToString() << ", DNS address="
	       << DnsAddr_;
    try {
	switch (UpdateMode_) {
	case DNSUPDATE_PTR:
	    Log(Cont) << ": Add PTR record." << LogEnd;
	    createSOAMsg();
	    addinMsg_delOldRR();
	    addinMsg_newPTR();
	    break;
	case DNSUPDATE_PTR_CLEANUP:
	    Log(Cont) << ": Cleanup PTR record." << LogEnd;
	    createSOAMsg();
	    addinMsg_delOldRR();
	    deletePTRRecordFromRRSet();
	    break;
	case DNSUPDATE_AAAA:
	    Log(Cont) << ": Add AAAA record." << LogEnd;
	    createSOAMsg();
	    addinMsg_delOldRR();
	    addinMsg_newAAAA();
	    break;
	case DNSUPDATE_AAAA_CLEANUP:
	    Log(Cont) << ": Cleanup AAAA record." << LogEnd;
	    createSOAMsg();
	    addinMsg_delOldRR();
	    deleteAAAARecordFromRRSet();
	    break;
	}
    }
    catch (const PException& p) {
        Log(Warning) << "DNS Update failed: " << p.message << LogEnd;
        return DNSUPDATE_ERROR;
    }

    if (Keyname_.length()>0) {
	// Add TSIG
	Message_->tsig_rr = tsig_record(domainname(Keyname_.c_str()), Fudge_,
				       domainname(Algorithm_.c_str()));
	Message_->sign_key = Key_;
    }

    try {
	sendMsg(timeout);
    }
    catch (const PException& p) {
	DnsUpdateResult result = DNSUPDATE_ERROR;
	if (strstr(p.message,"Could not connect TCP socket") ){
	    result = DNSUPDATE_CONNFAIL;
	}
	if (!strcmp(p.message,"NOTAUTH")){
	    Log(Error) << "DDNS: Nameserver returned NOTAUTH error." << LogEnd;
	    result = DNSUPDATE_SRVNOTAUTH;
	}

	Log(Error) << "DDNS: Update failed. Error: " << p.message << "." << LogEnd;
	return result;
     }// exeption catch

    return DNSUPDATE_SUCCESS;
}

/**
 * create new message for Dns Update
 *
 */
void DNSUpdate::createSOAMsg(){
    Message_ = new DnsMessage();
    Message_->OPCODE = OPCODE_UPDATE;
    Message_->questions.push_back(DnsQuestion(Zoneroot_, DNS_TYPE_SOA, CLASS_IN));
}

/**
 * insert a new-AAAA entry in message
 *
 */
void DNSUpdate::addinMsg_newAAAA(){
    DnsRR rr;
    rr.NAME = domainname(Hostname_.c_str(), Zoneroot_);
    rr.TYPE = qtype_getcode("AAAA", false);
    rr.TTL = txt_to_int(TTL_.c_str());
    string data = rr_fromstring(rr.TYPE, Hostip_.c_str(), Zoneroot_);
    rr.RDLENGTH = data.size();
    rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);

    Message_->authority.push_back(rr);
    Log(Info) << "DDNS: AAAA update:" << rr.NAME.tostring() << " -> " << Hostip_ << LogEnd;
}

void DNSUpdate::addDHCID(const char* duid, int duidlen) {
    DnsRR rr;
    rr.NAME = domainname(Hostname_.c_str(), Zoneroot_);
    rr.TYPE = qtype_getcode("DHCID", false);
    rr.TTL = txt_to_int(TTL_.c_str());

    char input_buf[512];
    char output_buf[35]; // identifier-type code (2) + digest type code (1) + digest (SHA-256 = 32 bytes)

    memcpy(input_buf, duid, duidlen); //
    memcpy(input_buf+duidlen, rr.NAME.c_str(), strlen((const char*)rr.NAME.c_str()) );

    sha256_buffer(input_buf, duidlen + strlen((const char*)rr.NAME.c_str() ),
		  output_buf+3);

    output_buf[0] = 0;
    output_buf[1] = 2; // identifier-type code: 0x0002 - DUID used as client identifier
    output_buf[2] = 1; // digest type = 1 (SHA-256)

    Message_->authority.push_back(rr);

}

/// @brief sets keys used for Transaction Signature (TSIG)
///
/// One can use dnssec-keygen from bind9 to generate such a key
///
/// @param keyname name of the key (e.g. "ddns-key")
/// @param base64encoded the actual key (e.g. 9SYMLnjK2ohb1N/56GZ5Jg==)
/// @param algo Algorithm name
/// @param fudge max difference between us signing and they are receiving
void DNSUpdate::setTSIG(const std::string& keyname, const std::string& base64encoded,
			const std::string& algo, uint32_t fudge) {
    Keyname_ = keyname;
    Key_ = base64encoded;
    Algorithm_ = algo;
    Fudge_ = fudge;
}

/**
 * delete a single rr from rrset. If no such RRs exist, then this Update RR will be
 * silently ignored by the primary master.
 *
 */
void DNSUpdate::deleteAAAARecordFromRRSet(){
  DnsRR rr;
  rr.NAME = domainname(Hostname_.c_str(), Zoneroot_);
  rr.TYPE = qtype_getcode("AAAA", false);
  rr.CLASS = QCLASS_NONE; /* 254 */
  rr.TTL = 0;
  string data = rr_fromstring(rr.TYPE, Hostip_.c_str(), Zoneroot_);
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  Message_->authority.push_back(rr);

  Log(Debug) << "DDNS: AAAA record created:" << rr.NAME.tostring() << " -> " << Hostip_ << LogEnd;
}

void DNSUpdate::deletePTRRecordFromRRSet(){

  DnsRR rr;
  const int bufSize = 128;
  char destination[16];
  char result[bufSize];
  memset(result, 0, bufSize);
  inet_pton6(Hostip_.c_str(), destination);
  doRevDnsAddress(destination,result);
  rr.NAME = result;
  rr.TYPE = qtype_getcode("PTR", false);
  rr.CLASS = QCLASS_NONE;
  rr.TTL = 0;
  string tmp  = string(Hostname_);
  string data = rr_fromstring(rr.TYPE, tmp.c_str());
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  Message_->authority.push_back(rr);

  Log(Info) << "DDNS: PTR record created: " << result << " -> " << tmp << LogEnd;
}

/**
 * insert a new-PTR entry in message
 *
 */
void DNSUpdate::addinMsg_newPTR(){
  DnsRR rr;
  const int bufSize = 128;
  char destination[16];
  char result[bufSize];
  memset(result, 0, bufSize);
  inet_pton6(Hostip_.c_str(), destination);
  doRevDnsAddress(destination,result);
  rr.NAME = result;
  rr.TYPE = qtype_getcode("PTR", false);
  rr.TTL = txt_to_int(TTL_.c_str());
  string tmp  = string(Hostname_);
  string data = rr_fromstring(rr.TYPE, tmp.c_str());
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  Message_->authority.push_back(rr);

  Log(Debug) << "DDNS: PTR record created: " << result << " -> " << tmp << LogEnd;
}

/**
 * insert a delete-RR entry in message for deleting old entry
 *
 */
void DNSUpdate::addinMsg_delOldRR(){
    //get old, available DnsRR from Dns Server
    DnsRR* oldDnsRR=this->get_oldDnsRR();
    if (oldDnsRR){
	//delete message
	oldDnsRR->CLASS = QCLASS_NONE; oldDnsRR->TTL = 0;
	Message_->authority.push_back(*oldDnsRR);
	delete oldDnsRR;
    }
}

/**
 * check hostname-RR entry is available in response message(xfr) from server
 *
 * @param msg - DnsMessage response from server
 * @param RemoteDnsRR - if DnsRR entry is available set RR record in RemoteDnsRR
 *
 * @return true - if RR record available, otherwise returns false
 */
bool DNSUpdate::DnsRR_avail(DnsMessage *msg, DnsRR& RemoteDnsRR){

    //check axfr_message
    bool flagSOA = false;

    if (msg->answers.empty()) {
	delete msg;
	return false;
    }

    stl_list(DnsRR)::iterator it = msg->answers.begin();

    it++;

    while (it != msg->answers.end()) {

	if (it->TYPE == DNS_TYPE_SOA) {
	    flagSOA = !flagSOA;
	}
	else {
	    if (!flagSOA) {

		if ( !strcmp(Hostname_.c_str(),(it->NAME.label(0)).c_str()) ){
		    RemoteDnsRR=*it;
		    delete msg;
		    return true;
		}
	    }
	}
	it++;
    }
    delete msg;

    return false;
}

/** get old RR entry from Dns Server */
DnsRR* DNSUpdate::get_oldDnsRR(){

    DnsRR* RemoteDnsRR= NULL;
    DnsMessage *q = NULL, *a = NULL;
    int sockid = -1;

    try {
	q = create_query(Zoneroot_, QTYPE_AXFR);

	pos_cliresolver res;
        /// @todo: Make this over UDP or TCP, not always TCP (TCP blocks on connect())
	_addr dnsAddr = ToPoslibAddr(DnsAddr_);
	sockid = res.tcpconnect(&dnsAddr);
	res.tcpsendmessage(q, sockid);

	res.tcpwaitanswer(a, sockid);
	if (!a) {
	    throw PException("tcpwaitanswer returned NULL");
	}
	if (a->RCODE != RCODE_NOERROR){

	    throw PException((char*)str_rcode(a->RCODE).c_str());
	}
	if (!a->answers.empty())
	    {
		RemoteDnsRR = new DnsRR();
		if (this->DnsRR_avail(a,*RemoteDnsRR) == false)
		    {
			delete RemoteDnsRR;
			RemoteDnsRR = NULL;
		    }
	    }

	if (a) {
	    // delete a; /// @todo: Why is this commented out? Memory leak!
	    a = 0;
	}
	if (q) {
	    delete q;
	    q = 0;
	}

	if (sockid != -1)
	    tcpclose(sockid);

	return RemoteDnsRR;

    } catch (const PException& p) {
	if (q) {
	    delete q;
	    q = NULL;
	}

	if (a) {
	    delete a;
	    a = NULL;
	}
	if (sockid != -1)
	    tcpclose(sockid);
	if (RemoteDnsRR)
	    delete RemoteDnsRR;
    Log(Error) << "DDNS: Attempt to get old DNS record failed:" << p.message << LogEnd;
	return 0;
    }
}

void DNSUpdate::sendMsg(unsigned int timeout) {
    switch (Proto_) {
    case DNSUPDATE_TCP:
	sendMsgTCP(timeout);
	return;
    case DNSUPDATE_UDP:
	sendMsgUDP(timeout);
	return;
    default:
	Log(Error) << "DDNS: Invalid protocol (non-TCP, non-UDP) specified." << LogEnd;
    }

    return;
}

/** send Update Message to server*/
void DNSUpdate::sendMsgTCP(unsigned int timeout){
    DnsMessage *a = NULL;
    int sockid = -1;

    try {
	pos_cliresolver res;
	res.tcp_timeout = timeout;
	_addr dnsAddr = ToPoslibAddr(DnsAddr_);
	sockid = res.tcpconnect(&dnsAddr);
	res.tcpsendmessage(Message_, sockid);
	res.tcpwaitanswer(a, sockid);
	if (a->RCODE != RCODE_NOERROR) {
	    throw PException((char*)str_rcode(a->RCODE).c_str());
	}
    } catch (const PException& p) {

	if (a)
	    delete a;
	if (sockid != -1)
	    tcpclose(sockid);

	throw PException(p.message);
    }

    if (a)
	delete a;
    if (sockid != -1)
	tcpclose(sockid);
}

void DNSUpdate::sendMsgUDP(unsigned int timeout) {
    DnsMessage *a = NULL;
    try {

	pos_cliresolver res;
	res.udp_tries[0] = timeout;
	res.n_udp_tries = 1; // just one timeout
	_addr dnsAddr = ToPoslibAddr(DnsAddr_);
	res.query(Message_, a, &dnsAddr, Q_NOTCP);
        if (!a) {
            throw PException("DNS server asnwer not received");
        }
	if (a->RCODE != RCODE_NOERROR) {
	    throw PException((char*)str_rcode(a->RCODE).c_str());
	}
    } catch (const PException& p) {

	if (a)
	    delete a;
	throw PException(p.message);
    }

    if (a)
	delete a;
}

/**
 * prints status reported by result
 *
 * @param result
 */
void DNSUpdate::showResult(int result)
{
    switch (result) {
    case DNSUPDATE_SUCCESS:
	if (UpdateMode_ == DNSUPDATE_AAAA || UpdateMode_ == DNSUPDATE_PTR)
	    Log(Debug) << "DDNS: DNS Update (add) successful." << LogEnd;
	else
	    Log(Debug) << "DDNS: DNS Update (delete) successful." << LogEnd;
	break;
    case DNSUPDATE_ERROR:
	Log(Warning) << "DDNS: DNS Update failed." << LogEnd;
	break;
    case DNSUPDATE_CONNFAIL:
	Log(Warning) << "DDNS: Unable to establish connection to the DNS server." << LogEnd;
	break;
    case DNSUPDATE_SRVNOTAUTH:
	Log(Warning) << "DDNS: DNS Update failed: server returned NOTAUTH." << LogEnd;
	break;
    case DNSUPDATE_SKIP:
	Log(Debug) << "DDNS: DNS Update was skipped." << LogEnd;
	break;
    }
}

std::string DNSUpdate::protoToString()
{
    switch(Proto_) {
    case DNSUPDATE_TCP:
	return "TCP";
    case DNSUPDATE_UDP:
	return "UDP";
    default:
    case DNSUPDATE_ANY:
	return "ANY";
    }
}
