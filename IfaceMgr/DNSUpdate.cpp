/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
 *          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
 *          from ENSEEIHT, Toulouse, France
 * changes: Krzysztof Wnuk keczi@poczta.onet.pl
 * released under GNU GPL v2 licence
 *
 * $Id: DNSUpdate.cpp,v 1.9 2006-08-27 21:24:35 thomson Exp $
 *
 */

#include "DNSUpdate.h"
#include "Portable.h"
#include "Logger.h"
#include <stdio.h>


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
  
   if (this->numberOfRecords == 1){ 
Log(Debug) << "Performing DNS Update: Only PTR record." << LogEnd;   
  try {
   this->createSOAMsg();
   this->addinMsg_delOldRR();   
   this->addinMsg_newPTR();
   this->sendMsg();
   
  } catch (PException p) {
	
    
      if (!strcmp(p.message,"Could not connect TCP socket") ){
		
	  Log(Error) << "DNS update : " << p.message << LogEnd;
	  return DNSUPDATE_CONNFAIL;
      }
      else if (!strcmp(p.message,"NOTAUTH")){
		
	  Log(Error) << "DNS update : Nameserver is not authoritative for this zone (" << p.message << ")" << LogEnd;	
	  return DNSUPDATE_SRVNOTAUTH;
      }
      else
      {		

	  Log(Error) << "DNS update : error not specified (" << p.message << ")" << LogEnd;
	  return DNSUPDATE_ERROR;
      } 
	
  }
   } // number of records 1 
 else if (this->numberOfRecords == 2){    
  
     Log(Debug) << "Performing DNS Cleanup: Only PTR record." << LogEnd;   
     try {
	 
	 this->createSOAMsg();
	 this->addinMsg_delOldRR();   
	 this->deletePTRRecordFromRRSet();
        
	 this->sendMsg();
     } catch (PException p) {
	 
	
	 if (!strcmp(p.message,"Could not connect TCP socket") ){

	     Log(Error) << "DNS update cleaning up PTR record: " << p.message << LogEnd;
	     return DNSUPDATE_CONNFAIL;
	 }
	 else if (!strcmp(p.message,"NOTAUTH")){

	     Log(Error) << "DNS update cleaning up PTR record: Nameserver is not authoritative for this zone (" << p.message << ")" << LogEnd;	
	     return DNSUPDATE_SRVNOTAUTH;
    }
	 else
	 {		

	     Log(Error) << "DNS update cleaning up PTR record: error not specified (" << p.message << ")" << LogEnd;
	     return DNSUPDATE_ERROR;
	 } 
	 
     }// exeption catch

   } // number of records 2
 else if (this->numberOfRecords == 3){    
  
     Log(Debug) << "Performing DNS Update: Only AAAA record." << LogEnd;   
     try {
	 
	 this->createSOAMsg();
	 this->addinMsg_delOldRR();   
	 this->addinMsg_newAAAA();
	 this->sendMsg();
     } catch (PException p) {
	 
	 
	 if (!strcmp(p.message,"Could not connect TCP socket") ){

	     Log(Error) << "DNS update : " << p.message << LogEnd;
	     return DNSUPDATE_CONNFAIL;
	 }
	 else if (!strcmp(p.message,"NOTAUTH")){

	     Log(Error) << "DNS update : Nameserver is not authoritative for this zone (" << p.message << ")" << LogEnd;	
	     return DNSUPDATE_SRVNOTAUTH;
    }
	 else
	 {		

	     Log(Error) << "DNS update : error not specified (" << p.message << ")" << LogEnd;
	     return DNSUPDATE_ERROR;
	 } 
	 
     }// exeption catch
     
 } // number of records 3


else if (this->numberOfRecords == 4){    
    
     Log(Debug) << "Performing DNS Cleanup: Only AAAA record." << LogEnd;   
     try {
	 
	 this->createSOAMsg();
	 this->addinMsg_delOldRR();   
	 this->deleteAAAARecordFromRRSet();
        //this->addinMsg_newAAAA();
	 this->sendMsg();
     } catch (PException p) {
	 
	
	 if (!strcmp(p.message,"Could not connect TCP socket") ){

	     Log(Error) << "DNS update : " << p.message << LogEnd;
	     return DNSUPDATE_CONNFAIL;
	 }
	 else if (!strcmp(p.message,"NOTAUTH")){

	     Log(Error) << "DNS update : Nameserver is not authoritative for this zone (" << p.message << ")" << LogEnd;	
	     return DNSUPDATE_SRVNOTAUTH;
    }
	 else
	 {		

	     Log(Error) << "DNS update : error not specified (" << p.message << ")" << LogEnd;
	     return DNSUPDATE_ERROR;
	 } 
	 
     }// exeption catch
     
 } // number of records 4
   


   
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
    Log(Debug) << "FQDN: Building AAAA record." << LogEnd;
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
  rr.CLASS = 254; // none 
  rr.TTL = 0;
  string data = rr_fromstring(rr.TYPE, hostip, *zoneroot);
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  //Log(Notice) << "Deleting old RR record for " << hostip << " in " << *zoneroot << LogEnd;
  message->authority.push_back(rr);

}

void DNSUpdate::deletePTRRecordFromRRSet(){

  
  DnsRR rr;
  rr.NAME = domainname ( "3.3.3.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.2.ip6.arpa.", *zoneroot);  // .0.0..0.0.0.0.0.2.ip6.arpa.
  stl_string dupa = rr.NAME.tostring();
  Log(Debug) << "rr.NAME=" << dupa << LogEnd;
  rr.CLASS=254;
  rr.TYPE = qtype_getcode("PTR", false);
  rr.TTL = 0;
  string data = rr_fromstring(rr.TYPE, "troi.example.com.", *zoneroot); 

  Log(Debug) << "rr.RDATA=" << data << LogEnd;
                                         
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  message->authority.push_back(rr);

}
		
/** insert a new-PTR entry in message*/
void DNSUpdate::addinMsg_newPTR(){
 

// this things in here are not working at all :(
	
  DnsRR rr;
  const int bufSize = 128;
  char destination[16];
  char result[bufSize];
  memset(result, 0, bufSize);
  inet_pton6(hostip, destination);
  //print_packed(destination);
  doRevDnsAddress(destination,result);

  rr.NAME = domainname ( result /*"3.3.3.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.2.ip6.arpa."*/, *zoneroot);  
                         // .0.0..0.0.0.0.0.2.ip6.arpa.
  stl_string dupa = rr.NAME.tostring();
  Log(Debug) << "rr.NAME=" << dupa << LogEnd;

  rr.TYPE = qtype_getcode("PTR", false);
  rr.TTL = txt_to_int(ttl);
  string data = rr_fromstring(rr.TYPE, "troi.example.com.", *zoneroot); 

  Log(Debug) << "#### rr.RDATA=" << data << LogEnd;
                                         
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
  message->authority.push_back(rr);
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
