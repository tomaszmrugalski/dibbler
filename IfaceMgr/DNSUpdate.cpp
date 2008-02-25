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

DNSUpdate::DNSUpdate(string dns_address, string zonename,string hostname,string hostip,
		     DnsUpdateMode updateMode) { 
    message= NULL;
    
    if (updateMode==DNSUPDATE_AAAA || updateMode==DNSUPDATE_AAAA_CLEANUP) {
	this->splitHostDomain(hostname);
    } else {
	this->hostname=new char[hostname.length()+1];
	strcpy(this->hostname,hostname.c_str());
	zoneroot = new domainname(zonename.c_str());
    }
    
    txt_to_addr(&server,dns_address.c_str());
    this->hostip = new char[hostip.length()+1];
    strcpy(this->hostip,hostip.c_str());
    this->ttl=new char[strlen(DNSUPDATE_DEFAULT_TTL)+1];
    strcpy(this->ttl,DNSUPDATE_DEFAULT_TTL);
    this->updateMode = updateMode;
}
 
DNSUpdate::~DNSUpdate() {
    if (message)
	delete message;
    delete zoneroot;
    delete [] hostip;
    delete [] hostname;
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
    if (dotpos == string::npos) {
	Log(Warning) << "Name provided for DNS update is not a FQDN. [" << fqdnName
		     << "]." << LogEnd;
	this->hostname = new char[fqdnName.length()+1];
	strcpy(this->hostname, fqdnName.c_str());
    }
    else {
	string hostname = fqdnName.substr(0, dotpos);
	string domain = fqdnName.substr(dotpos + 1, fqdnName.length() - dotpos - 1);
	this->hostname = new char[hostname.length()];
	strcpy(this->hostname, hostname.c_str());
	this->zoneroot = new domainname(domain.c_str());
    }
}


DnsUpdateResult DNSUpdate::run(){

    switch (this->updateMode) {
    case DNSUPDATE_PTR:
	Log(Debug) << "FQDN: Performing DNS Update: Only PTR record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->addinMsg_newPTR();
	break;
    case DNSUPDATE_PTR_CLEANUP:
	Log(Debug) << "FQDN: Performing DNS Cleanup: Only PTR record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->deletePTRRecordFromRRSet();
	break;
    case DNSUPDATE_AAAA:
	Log(Debug) << "FQDN: Performing DNS Update: Only AAAA record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->addinMsg_newAAAA();
	break;
    case DNSUPDATE_AAAA_CLEANUP:
	Log(Debug) << "FQDN: Performing DNS Cleanup: Only AAAA record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->deleteAAAARecordFromRRSet();
	break;
    }
    
    try {
	this->sendMsg();
    } 
    catch (PException p) {
	if (!strcmp(p.message,"Could not connect TCP socket") ){
	    Log(Error) << "FQDN: Unable to establish connection to the DNS server:" << p.message << LogEnd;
	    return DNSUPDATE_CONNFAIL;
	}
	if (!strcmp(p.message,"NOTAUTH")){
	    Log(Error) << "FQDN: Nameserver is not authoritative for this zone (" << p.message << ")" << LogEnd;	
	    return DNSUPDATE_SRVNOTAUTH;
	}

	Log(Error) << "FQDN: Error not specified (" << p.message << ")" << LogEnd;
	return DNSUPDATE_ERROR;
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
    rr.NAME = domainname(hostname, *zoneroot);
    rr.TYPE = qtype_getcode("AAAA", false);
    rr.TTL = txt_to_int(ttl);
    string data = rr_fromstring(rr.TYPE, hostip, *zoneroot);
    rr.RDLENGTH = data.size();
    rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
    
    message->authority.push_back(rr);
    Log(Debug) << "FQDN: AAAA record created:" << rr.NAME.tostring() << " -> " << hostip << LogEnd;
}

/** 
 * delete a single rr from rrset. If no such RRs exist, then this Update RR will be
 * silently ignored by the primary master.
 * 
 */
void DNSUpdate::deleteAAAARecordFromRRSet(){
  DnsRR rr;
  rr.NAME = domainname(hostname, *zoneroot);
  rr.TYPE = qtype_getcode("AAAA", false);
  rr.CLASS = QCLASS_NONE; /* 254 */
  rr.TTL = 0;
  string data = rr_fromstring(rr.TYPE, hostip, *zoneroot);
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  message->authority.push_back(rr);

  Log(Debug) << "FQDN: AAAA record created:" << rr.NAME.tostring() << " -> " << hostip << LogEnd;
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
  string tmp  = hostname;
  string data = rr_fromstring(rr.TYPE, tmp.c_str()); 
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  message->authority.push_back(rr);

  Log(Debug) << "FQDN: PTR record created: " << result << " -> " << tmp << LogEnd;
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
  string tmp  = hostname;
  string data = rr_fromstring(rr.TYPE, tmp.c_str()); 
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  message->authority.push_back(rr);

  Log(Debug) << "FQDN: PTR record created: " << result << " -> " << tmp << LogEnd;
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
  
  
  if (msg->answers.empty()) { delete msg; return false; }
  
  stl_list(DnsRR)::iterator it = msg->answers.begin();
  
  it++;
     
  while (it != msg->answers.end()) {

   if (it->TYPE == DNS_TYPE_SOA) {
       flagSOA = !flagSOA;
   } 
   else {
    if (!flagSOA) {
     
     if ( !strcmp( hostname,(it->NAME.label(0)).c_str()) ){
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
 
 
/** get old RR entry from Dns Server*/
 
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
       //delete a;
       a = 0;
   }
   if (q) {
       delete q;
       q = 0;
   }
   
   if (sockid != -1) tcpclose(sockid);
   
   return RemoteDnsRR;
   
  } catch (PException p) {
      if (q) delete q;
      if (a) delete a;     
      if (sockid != -1) tcpclose(sockid);
      if (RemoteDnsRR) delete RemoteDnsRR;
      Log(Error) << "FQDN: Attempt to get old DNS record failed." << LogEnd;
      return 0;
  } 
}
 
/** send Update Message to server*/
void DNSUpdate::sendMsg(){
    DnsMessage *a = NULL;
    int sockid = -1;
    
    try {
	
	pos_cliresolver res;
	sockid = res.tcpconnect(&server);
	res.tcpsendmessage(message, sockid);
	res.tcpwaitanswer(a, sockid);
	if (a->RCODE != RCODE_NOERROR){
	 
	    throw PException((char*)str_rcode(a->RCODE).c_str());
	}
	else{			
	
	}		
    } catch (PException p) {
	

	
	if (a) delete a;
	if (sockid != -1) tcpclose(sockid);
	
	throw PException(p.message);
	
    }
	
    if (a) delete a;
    if (sockid != -1) tcpclose(sockid);
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
	    Log(Notice) << "FQDN: DNS Update (add) successful." << LogEnd;
	else
	    Log(Notice) << "FQDN: DNS Update (delete) successful." << LogEnd;
	break;
    case DNSUPDATE_ERROR:
	Log(Warning) << "FQDN: DNS Update failed." << LogEnd;
	break;
    case DNSUPDATE_CONNFAIL:
	Log(Warning) << "FQDN: Unable to establish connection to the DNS server." << LogEnd;
	break;
    case DNSUPDATE_SRVNOTAUTH:
	Log(Warning) << "FQDN: DNS Update failed: server is not not authoritative." << LogEnd;
	break;
    case DNSUPDATE_SKIP:
	Log(Notice) << "FQDN: DNS Update was skipped." << LogEnd;
	break;
    }
}
