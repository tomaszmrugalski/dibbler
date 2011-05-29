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
 * $Id: DNSUpdate.cpp,v 1.21 2008-02-25 17:49:08 thomson Exp $
 *
 */

#include "DNSUpdate.h"
#include "Portable.h"
#include "Logger.h"
#include <stdio.h>

DNSUpdate::DNSUpdate(string dns_address, string zonename, string hostname,
		     string hostip, DnsUpdateMode updateMode,
		     DnsUpdateProtocol proto /* = DNSUPDATE_TCP */)
    :_hostname(NULL) { 
    message= NULL;
    memset(&server, 0, sizeof(server));
    server.ss_family = AF_INET6;
    
    if (updateMode==DNSUPDATE_AAAA || updateMode==DNSUPDATE_AAAA_CLEANUP) {
	splitHostDomain(hostname);
    } else {
	_hostname=new char[hostname.length()+1];
	strcpy(_hostname,hostname.c_str());
	zoneroot = new domainname(zonename.c_str());
    }
    
    txt_to_addr(&server,dns_address.c_str());
    this->hostip = new char[hostip.length()+1];
    strcpy(this->hostip,hostip.c_str());
    this->ttl=new char[strlen(DNSUPDATE_DEFAULT_TTL)+1];
    strcpy(this->ttl,DNSUPDATE_DEFAULT_TTL);
    this->updateMode = updateMode;
    _proto = proto;
}
 
DNSUpdate::~DNSUpdate() {
    if (message)
	delete message;
    delete zoneroot;
    delete [] hostip;
    delete [] _hostname;
    delete [] ttl;
}

/** 
 * this function splits fqdn (e.g. malcolm.example.com) into hostname (e.g. malcolm) and domain (e.g. example.com)
 * 
 * @param fqdnName 
 */
void DNSUpdate::splitHostDomain(string fqdnName) {
    std::string::size_type dotpos = fqdnName.find(".");
    string hostname = "";
    string domain = "";
    if (_hostname) {
	delete [] _hostname;
	_hostname = 0;
    }

    if (dotpos == string::npos) {
	Log(Warning) << "Name provided for DNS update is not a FQDN. [" << fqdnName
		     << "]." << LogEnd;
	_hostname = new char[fqdnName.length()+1];
	strcpy(_hostname, fqdnName.c_str());
    }
    else {
	string hostname = fqdnName.substr(0, dotpos);
	string domain = fqdnName.substr(dotpos + 1, fqdnName.length() - dotpos - 1);
	_hostname = new char[hostname.length()+1];
	strncpy(_hostname, hostname.c_str(), hostname.length());
	_hostname[hostname.length()] = 0; // make sure it is null terminated
	this->zoneroot = new domainname(domain.c_str());
    }
}

DnsUpdateResult DNSUpdate::run(int timeout){

    switch (this->updateMode) {
    case DNSUPDATE_PTR:
	Log(Debug) << "DDNS: Performing DNS Update: Only PTR record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->addinMsg_newPTR();
	break;
    case DNSUPDATE_PTR_CLEANUP:
	Log(Debug) << "DDNS: Performing DNS Cleanup: Only PTR record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->deletePTRRecordFromRRSet();
	break;
    case DNSUPDATE_AAAA:
	Log(Debug) << "DDNS: Performing DNS Update: Only AAAA record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->addinMsg_newAAAA();
	break;
    case DNSUPDATE_AAAA_CLEANUP:
	Log(Debug) << "DDNS: Performing DNS Cleanup: Only AAAA record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->deleteAAAARecordFromRRSet();
	break;
    }
    
    try {
	sendMsg(timeout);
    } 
    catch (PException p) {
	DnsUpdateResult result = DNSUPDATE_ERROR;
	if (strstr(p.message,"Could not connect TCP socket") ){
	    result = DNSUPDATE_CONNFAIL;
	} 
	if (!strcmp(p.message,"NOTAUTH")){
	    Log(Error) << "DDNS: Nameserver returned NOTAUTH error." << LogEnd;	
	    result = DNSUPDATE_SRVNOTAUTH;
	}

	Log(Error) << "DDNS error: " << p.message << "." << LogEnd;
	return result;
     }// exeption catch

    return DNSUPDATE_SUCCESS;
}

/** 
 * create new message for Dns Update
 * 
 */
void DNSUpdate::createSOAMsg(){
    message = new DnsMessage();
    message->OPCODE = OPCODE_UPDATE;
    message->questions.push_back(DnsQuestion(*zoneroot, DNS_TYPE_SOA, CLASS_IN));
}

/** 
 * insert a new-AAAA entry in message
 * 
 */
void DNSUpdate::addinMsg_newAAAA(){
    DnsRR rr;
    rr.NAME = domainname(_hostname, *zoneroot);
    rr.TYPE = qtype_getcode("AAAA", false);
    rr.TTL = txt_to_int(ttl);
    string data = rr_fromstring(rr.TYPE, hostip, *zoneroot);
    rr.RDLENGTH = data.size();
    rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
    
    message->authority.push_back(rr);
    Log(Debug) << "DDNS: AAAA record created:" << rr.NAME.tostring() << " -> " << hostip << LogEnd;
}

/** 
 * delete a single rr from rrset. If no such RRs exist, then this Update RR will be
 * silently ignored by the primary master.
 * 
 */
void DNSUpdate::deleteAAAARecordFromRRSet(){
  DnsRR rr;
  rr.NAME = domainname(_hostname, *zoneroot);
  rr.TYPE = qtype_getcode("AAAA", false);
  rr.CLASS = QCLASS_NONE; /* 254 */
  rr.TTL = 0;
  string data = rr_fromstring(rr.TYPE, hostip, *zoneroot);
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  message->authority.push_back(rr);

  Log(Debug) << "DDNS: AAAA record created:" << rr.NAME.tostring() << " -> " << hostip << LogEnd;
}

void DNSUpdate::deletePTRRecordFromRRSet(){

  DnsRR rr;
  const int bufSize = 128;
  char destination[16];
  char result[bufSize];
  memset(result, 0, bufSize);
  inet_pton6(hostip, destination);
  doRevDnsAddress(destination,result);
  rr.NAME = result;
  rr.TYPE = qtype_getcode("PTR", false);
  rr.CLASS = QCLASS_NONE;
  rr.TTL = 0;
  string tmp  = string(_hostname);
  string data = rr_fromstring(rr.TYPE, tmp.c_str()); 
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  message->authority.push_back(rr);

  Log(Debug) << "DDNR: PTR record created: " << result << " -> " << tmp << LogEnd;
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
  inet_pton6(hostip, destination);
  doRevDnsAddress(destination,result);
  rr.NAME = result;
  rr.TYPE = qtype_getcode("PTR", false);
  rr.TTL = txt_to_int(ttl);
  string tmp  = string(_hostname);
  string data = rr_fromstring(rr.TYPE, tmp.c_str()); 
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  message->authority.push_back(rr);

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
	message->authority.push_back(*oldDnsRR);   
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
		
		if ( !strcmp(_hostname,(it->NAME.label(0)).c_str()) ){
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
	q = create_query(*zoneroot, QTYPE_AXFR);
	
	pos_cliresolver res;   
	sockid = res.tcpconnect(&server);  
	res.tcpsendmessage(q, sockid);
	
	res.tcpwaitanswer(a, sockid);
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
	    // delete a; // FIXME: Why is this commented out? Memory leak!
	    a = 0;
	}
	if (q) {
	    delete q;
	    q = 0;
	}
	
	if (sockid != -1) 
	    tcpclose(sockid);
	
	return RemoteDnsRR;
	
    } catch (PException p) {
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
	Log(Error) << "DDNS: Attempt to get old DNS record failed." << LogEnd;
	return 0;
    } 
}

void DNSUpdate::sendMsg(unsigned int timeout) {
    switch (_proto) {
    case DNSUPDATE_TCP:
	Log(Debug) << "DDNS: Updating over TCP." << LogEnd;
	sendMsgTCP(timeout);
	return;
    case DNSUPDATE_UDP:
	Log(Debug) << "DDNS: Updating over UDP." << LogEnd;
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
	sockid = res.tcpconnect(&server);
	res.tcpsendmessage(message, sockid);
	res.tcpwaitanswer(a, sockid);
	if (a->RCODE != RCODE_NOERROR) {
	    throw PException((char*)str_rcode(a->RCODE).c_str());
	}
    } catch (PException p) {
	
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
	res.query(message, a, &server, Q_NOTCP);
	if (a->RCODE != RCODE_NOERROR) {
	    throw PException((char*)str_rcode(a->RCODE).c_str());
	}
    } catch (PException p) {
	
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
	if (this->updateMode == DNSUPDATE_AAAA || this->updateMode == DNSUPDATE_PTR)
	    Log(Notice) << "DDNS: DNS Update (add) successful." << LogEnd;
	else
	    Log(Notice) << "DDNS: DNS Update (delete) successful." << LogEnd;
	break;
    case DNSUPDATE_ERROR:
	Log(Warning) << "DDNS: DNS Update failed." << LogEnd;
	break;
    case DNSUPDATE_CONNFAIL:
	Log(Warning) << "DDNS: Unable to establish connection to the DNS server." << LogEnd;
	break;
    case DNSUPDATE_SRVNOTAUTH:
	Log(Warning) << "DDNS: DNS Update failed: server is not not authoritative." << LogEnd;
	break;
    case DNSUPDATE_SKIP:
	Log(Notice) << "DDNS: DNS Update was skipped." << LogEnd;
	break;
    }
}
