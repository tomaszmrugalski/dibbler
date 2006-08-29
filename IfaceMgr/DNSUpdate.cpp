/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
 *          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
 *          from ENSEEIHT, Toulouse, France
 * changes: Krzysztof Wnuk keczi@poczta.onet.pl
 * released under GNU GPL v2 licence
 *
 * $Id: DNSUpdate.cpp,v 1.11 2006-08-29 00:54:29 thomson Exp $
 *
 */

#include "DNSUpdate.h"
#include "Portable.h"
#include "Logger.h"
#include <stdio.h>

/**
 * @file   DNSUpdate.cpp
 * @author 
 * @date   Tue Aug 29 00:59:35 2006
 * 
 * @brief  
 * 
 * this object is created in following places:
 * - ClntIfaceMgr.cpp:274
 * - ClntMsgRelease.cpp:119
 * - SrvMsgReply.cpp: (fqdnRelease method): 873, 911, 927
 * - SrvMsg.cpp (prepareFQDN method):  547, 579, 607
 */

DNSUpdate::DNSUpdate(char* dns_address,char*zonename,char* hostname,char* hostip,char* ttl, 
		     int numberOfRecords) { 
  message= NULL;
	
  txt_to_addr(&server,dns_address);
  this->hostip = new char[strlen(hostip)+1];
  strcpy(this->hostip,hostip);
  this->hostname=new char[strlen(hostname)+1];
  strcpy(this->hostname,hostname);
  zoneroot = new domainname(zonename);
  this->ttl=new char[strlen(ttl)+1];
  strcpy(this->ttl,ttl);
  this->numberOfRecords = numberOfRecords;
  
}
 
DNSUpdate::~DNSUpdate() {
  delete message;
  delete zoneroot;
  delete [] hostip;
  delete [] hostname;
  delete [] ttl;
}

DnsUpdateResult DNSUpdate::run(){

    switch (this->numberOfRecords) {
    case 1:
	Log(Debug) << "Performing DNS Update: Only PTR record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->addinMsg_newPTR();
	break;
    case 2:
	// FIXME: This does not work
	Log(Debug) << "Performing DNS Cleanup: Only PTR record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->deletePTRRecordFromRRSet();
	break;
    case 3:
	Log(Debug) << "Performing DNS Update: Only AAAA record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->addinMsg_newAAAA();
	break;
    case 4:
	Log(Debug) << "Performing DNS Cleanup: Only AAAA record." << LogEnd;   
	this->createSOAMsg();
	this->addinMsg_delOldRR();   
	this->deleteAAAARecordFromRRSet();
        //this->addinMsg_newAAAA();
	break;
    }
    
    try {
	this->sendMsg();
    } catch (PException p) {
	if (!strcmp(p.message,"Could not connect TCP socket") ){
	    Log(Error) << "DNS update : " << p.message << LogEnd;
	    return DNSUPDATE_CONNFAIL;
	}
	if (!strcmp(p.message,"NOTAUTH")){
	    Log(Error) << "DNS update : Nameserver is not authoritative for this zone (" << p.message << ")" << LogEnd;	
	    return DNSUPDATE_SRVNOTAUTH;
	}

	Log(Error) << "DNS update : error not specified (" << p.message << ")" << LogEnd;
	return DNSUPDATE_ERROR;
     }// exeption catch

    return DNSUPDATE_SUCCESS;
}

/** create new message for Dns Update*/

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

/** insert a delete-RR entry in message for deleting old entry */

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
       //printf("There was a SOA flag in the answer !!!!!!, changed to on SOA \n");
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
  
   
   a = NULL;
   delete q; 
   q = NULL;  
   delete q;   
   
   if (sockid != -1) tcpclose(sockid);
   
   return RemoteDnsRR;
   
  } catch (PException p) {
    
      if (q) delete q;
      if (a) delete a;     
      if (sockid != -1) tcpclose(sockid);
      if (RemoteDnsRR) delete RemoteDnsRR;
      throw PException(p);
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
