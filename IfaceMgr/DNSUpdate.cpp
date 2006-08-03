/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
 *          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
 *          from ENSEEIHT, Toulouse, France
 * changes: Krzysztof Wnuk keczi@poczta.onet.pl
 * released under GNU GPL v2 licence
 *
 * $Id: DNSUpdate.cpp,v 1.6 2006-08-03 00:44:44 thomson Exp $
 *
 */

#include "DNSUpdate.h"
#include "Logger.h"
#include <stdio.h>

// FIXME: this file is one huge mess. :)
// FIXME: used Log(...) instead of printf()
// FIXME: Write comments in english only. Polish, french and german are NOT accepted languages.

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
  try {
   printf("Updating only one record !!! \n");   
   this->createSOAMsg();
   
   // method delete OldRR
   this->addinMsg_delOldRR();   
   
   /// methode add
   this->addinMsg_newPTR();
            
   //sendmessage
   this->sendMsg();
   
  } catch (PException p) {
	
      //printf("Failed to start update operation\n");
      if (!strcmp(p.message,"Could not connect TCP socket") ){
// 		printf("Error: %s\n",p.message);
	  Log(Error) << "DNS update : " << p.message << LogEnd;
	  return DNSUPDATE_CONNFAIL;
      }
      else if (!strcmp(p.message,"NOTAUTH")){
// 		printf("Nameserver is not authoritative for this zone %s\n", p.message);
	  Log(Error) << "DNS update : Nameserver is not authoritative for this zone (" << p.message << ")" << LogEnd;	
	  return DNSUPDATE_SRVNOTAUTH;
      }
      else
      {		
//    		printf("Not specified error: %s \n",p.message);
	  Log(Error) << "DNS update : error not specified (" << p.message << ")" << LogEnd;
	  return DNSUPDATE_ERROR;
      } 
	
  }
   } // number of records 1 
 else if (this->numberOfRecords == 2){    
  try {
   Log(Debug) << "Performing DNS Update: Both (AAAA and PTR) records." << LogEnd;
   Log(Debug) << "Updating PTR record: not supported yet, skipped." << LogEnd;

#if 0
   // for a while this is not working ... 
   this->createSOAMsg();
   // method delete OldRR
   this->addinMsg_delOldRR();   
   
   /// methode add
   this->addinMsg_newPTR();
   //sendmessage
   this->sendMsg();
#endif

   // Adding AAAA record
   Log(Debug) << "Updating AAAA record." << LogEnd;
   this->createSOAMsg();
   this->addinMsg_delOldRR();   
   this->addinMsg_newAAAA();
   this->sendMsg();
  } catch (PException p) {
	
	//printf("Failed to start update operation\n");
	if (!strcmp(p.message,"Could not connect TCP socket") ){
// 		printf("Error: %s\n",p.message);
		Log(Error) << "DNS update : " << p.message << LogEnd;
		return DNSUPDATE_CONNFAIL;
	}
	else if (!strcmp(p.message,"NOTAUTH")){
// 		printf("Nameserver is not authoritative for this zone %s\n", p.message);
		Log(Error) << "DNS update : Nameserver is not authoritative for this zone (" << p.message << ")" << LogEnd;	
		return DNSUPDATE_SRVNOTAUTH;
    }
	else
	{		
//    		printf("Not specified error: %s \n",p.message);
		Log(Error) << "DNS update : error not specified (" << p.message << ")" << LogEnd;
		return DNSUPDATE_ERROR;
    	} 
	
  	}// exeption catch

   } // number of records 2
 else if (this->numberOfRecords == 3){    
     // used on the client side. Perform AAAA update only
     try {
	 
	 this->createSOAMsg();
	 this->addinMsg_delOldRR();   
	 this->addinMsg_newAAAA();
	 this->sendMsg();
     } catch (PException p) {
	 
	 //printf("Failed to start update operation\n");
	 if (!strcmp(p.message,"Could not connect TCP socket") ){
// 		printf("Error: %s\n",p.message);
	     Log(Error) << "DNS update : " << p.message << LogEnd;
	     return DNSUPDATE_CONNFAIL;
	 }
	 else if (!strcmp(p.message,"NOTAUTH")){
// 		printf("Nameserver is not authoritative for this zone %s\n", p.message);
	     Log(Error) << "DNS update : Nameserver is not authoritative for this zone (" << p.message << ")" << LogEnd;	
	     return DNSUPDATE_SRVNOTAUTH;
    }
	 else
	 {		
//    		printf("Not specified error: %s \n",p.message);
	     Log(Error) << "DNS update : error not specified (" << p.message << ")" << LogEnd;
	     return DNSUPDATE_ERROR;
	 } 
	 
     }// exeption catch
     
 } // number of records 2
   
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
}

/** insert a new-PTR entry in message*/
void DNSUpdate::addinMsg_newPTR(){
  DnsRR rr;
  rr.NAME = domainname(hostname, *zoneroot); // this is not working also ... 
  rr.TYPE = qtype_getcode("PTR", false);
  rr.TTL = txt_to_int(ttl);
  string data = rr_fromstring(rr.TYPE, hostip, *zoneroot); // tu jest problem wg mnie 
/** 
    TU jest problem, jezeli zostawimy tak jak jest to dibbler stwierdzi ze DNS zostal 
    zaktualizowany ale zapytanie typu nslookup -type=PTR <adres> nie zwroci rekordu.
    Nie wiem za bardzo co powinny byc w polu data, w dokumentacji biblioteki posliba 
    doczytalem sie ze maja to byc dane odpowiednie do rodzaju rekordu, czyli na pewno 
    nie bede to samo co AAAA tylko pytanie co.  Skrobnolem maila to autora posliba ale 
    narazie sie nie odezwal. Ponizej opis wyciety z pliku rr.h w poslibie.
*/

/*!
 * \brief convert a string to binary RR data
 *
 * This function converts a string describing a Resource Record to binary RR
 * data. The string should be in master file format - that is, if multiple
 * arguments are to be put in the RR data, they should be separated by any
 * number of spaces and tabs. For example, MX data might be "10 mail.yo.net.".
 * You can specify an origin to which domain names are considered relative by
 * means of the origin parameter.
 * \param RRTYTPE Type of the RR
 * \param data The text describing the RR
 * \param origin If given, the domain name relative domain names are considered
                 relative to. This should be a binary domain name, like the
                 domainname::domain field. If not given, domain names are
                 considered relative to the root domain.
 * \return Binary data describing the RR
 * \sa rr_tostring()
 */
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

/**check hostname-RR entry is available in response message(xfr) from server
 
 param msg - DnsMessage response from server
 param RemoteDnsRR - if DnsRR entry is available set RR record in RemoteDnsRR
      
 return true - if RR record available 
      else false 
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
    if (flagSOA) {
     
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
	  
   //printf("Connecting...send AXFR-message\n");   
   q = create_query(*zoneroot, QTYPE_AXFR);
   
   pos_cliresolver res;   
   sockid = res.tcpconnect(&server);  
   res.tcpsendmessage(q, sockid);
   
   
   res.tcpwaitanswer(a, sockid);
   if (a->RCODE != RCODE_NOERROR){    
    //printf("Zone transfer returned error code: %s\n", str_rcode(a->RCODE).c_str());
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
      //printf("Error: %s\n", p.message);
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
	//printf("Connecting...send update message\n");
	pos_cliresolver res;
	sockid = res.tcpconnect(&server);
	//printf("Connected to server; sending message...\n");
	res.tcpsendmessage(message, sockid);
	//printf("Message sent; waiting for answer...\n");
	res.tcpwaitanswer(a, sockid);
	if (a->RCODE != RCODE_NOERROR){
	    //printf("Zone transfer returned error code: %s\n", str_rcode(a->RCODE).c_str());
	    throw PException((char*)str_rcode(a->RCODE).c_str());
	}
	else{			
	    //printf("Zone succesfully updated.\n");
	}		
    } catch (PException p) {
	
	//printf(("Error: %s\n", p.message));
	
	if (a) delete a;
	if (sockid != -1) tcpclose(sockid);
	
	throw PException(p.message);
	
    }
	
    if (a) delete a;
    if (sockid != -1) tcpclose(sockid);
}
