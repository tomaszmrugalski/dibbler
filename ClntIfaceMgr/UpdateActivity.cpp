/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
 *          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
 *          from ENSEEIHT, Toulouse, France
 *
 * released under GNU GPL v2 licence
 *
 * $Id: UpdateActivity.cpp,v 1.3 2006-05-01 13:35:40 thomson Exp $
 *
 */

#include "UpdateActivity.h"
#include "Logger.h"
#include <stdio.h>

UpdateActivity::UpdateActivity(char* dns_address,char*zonename,char* hostname,char* hostip,char* ttl){ 
  
  message= NULL;
  /* test
  txt_to_addr(&server,"::1");
  hostip = new char(33);
  strcpy(hostip,"::123");
  hostname=new char(30);
  strcpy(hostname,"toto");
  zoneroot = new domainname("my.domain.org");*/
	
  txt_to_addr(&server,dns_address);
  this->hostip = new char(sizeof(hostip));
  strcpy(this->hostip,hostip);
  this->hostname=new char(sizeof(hostname));
  strcpy(this->hostname,hostname);
  zoneroot = new domainname(zonename);
  this->ttl=new char(sizeof(ttl));
  strcpy(this->ttl,ttl);
  
}
 
UpdateActivity::~UpdateActivity() {
  delete message;
  delete zoneroot;
  delete hostip;
  delete hostname;
  delete ttl;
}


/** main method for updating 

return     DNSUPDATE_SUCCESS(0) no errors
		   DNSUPDATE_ERROR(1) not specified error
		   DNSUPDATE_CONNFAIL(2) server isn't available
		   DNSUPDATE_SRVNOTAUTH(3) server is not authoritve for zone
  
*/
u_int UpdateActivity::run(){
  
      
  try {
      
   this->createSOAMsg();
   
   // method delete OldRR
   this->addinMsg_delOldRR();   
   
   /// methode add
   this->addinMsg_newRR();
            
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
   
  return DNSUPDATE_SUCCESS;
}

/** create new message for Dns Update*/

void UpdateActivity::createSOAMsg(){
  message = new DnsMessage();
  message->OPCODE = OPCODE_UPDATE;
  message->questions.push_back(DnsQuestion(*zoneroot, DNS_TYPE_SOA, CLASS_IN));
   
}
/** insert a new-RR entry in message*/

void UpdateActivity::addinMsg_newRR(){
  DnsRR rr;
  rr.NAME = domainname(hostname, *zoneroot);
  rr.TYPE = qtype_getcode("AAAA", false);
  rr.TTL = txt_to_int(ttl);
  string data = rr_fromstring(rr.TYPE, hostip, *zoneroot);
  rr.RDLENGTH = data.size();
  rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);
    
  message->authority.push_back(rr);
}

/** insert a delete-RR entry in message for deleting old entry */

void UpdateActivity::addinMsg_delOldRR(){
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

bool UpdateActivity::DnsRR_avail(DnsMessage *msg, DnsRR& RemoteDnsRR){
  
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
 
DnsRR* UpdateActivity::get_oldDnsRR(){
 
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
void UpdateActivity::sendMsg(){
   
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
