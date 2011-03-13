/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 * 
 * released under GNU GPL v2 only licence
 *
 */

#include <string.h>
#include "Portable.h"
#include "DHCPConst.h"
#include "OptIAPrefix.h"

TOptIAPrefix::TOptIAPrefix( char * &buf, int &n, TMsg* parent)
	:TOpt(OPTION_IAPREFIX, parent)
{
    	
	this->ValidOpt=false;
    if (n>=25) // was 24 for IA address
    {
       
	this->Pref  = ntohl(*((long*)buf));
        buf+= 4;  n-=4;
        this->Valid = ntohl(*((long*)buf));
        buf+= 4;  n-=4;    
	this->PrefixLength  = *buf;// was ntohl(*((char*)buf));
        buf+= 1;  n-=1;
        this->Prefix=new TIPv6Addr(buf); // was buf
        buf+= 16; n-=16;
        this->ValidOpt=true;

}
}

TOptIAPrefix::TOptIAPrefix(SPtr<TIPv6Addr> prefix, char prefixLength, unsigned long pref, unsigned long valid,
				 TMsg* parent)
    :TOpt(OPTION_IAPREFIX, parent) {
    /*if(prefix)
        Prefix=prefix;
    else*/// we are not checking is prefix is a proper address type, 
    Prefix=prefix;
    this->Pref = pref;
    this->Valid = valid;
    this->PrefixLength = prefixLength;
	
}
int TOptIAPrefix::getSize() {
    int mySize = 29; // was 28 for Option IAAddress so, this should be 29, no idea why 
    return mySize+getSubOptSize();
}

void TOptIAPrefix::setPref(unsigned long pref) {
    this->Pref = pref;
}

void TOptIAPrefix::setValid(unsigned long valid) {
    this->Valid = valid;
}
void TOptIAPrefix::setPrefixLenght(char prefix_length){
    this->PrefixLength = prefix_length;
}

char * TOptIAPrefix::storeSelf( char* buf)
{
	
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons( getSize()-4 );
    buf+=2;
    *(uint32_t*)buf = htonl(Pref);
    buf+=4;
    *(uint32_t*)buf = htonl(Valid);
    buf+=4;
    *(char*)buf = PrefixLength;
    buf+=1;
    memcpy(buf,Prefix->getAddr(),16);
    buf+=16;
       
    buf=storeSubOpt(buf);
    return buf;
}

 SPtr<TIPv6Addr> TOptIAPrefix::getPrefix()
{
	return this->Prefix;
}

unsigned long TOptIAPrefix::getPref()
{
	return this->Pref;
}

unsigned long TOptIAPrefix::getValid()
{
	return this->Valid;
}
char TOptIAPrefix::getPrefixLength(){
	return this->PrefixLength;
}
bool TOptIAPrefix::isValid()
{
    return this->ValidOpt;
}
