#include <iostream>
#include <strstream>
#include <time.h>

#include "Portable.h"
#include "AddrAddr.h"
#include "DHCPConst.h"

//addr is in packed form (char addr[16])
TAddrAddr::TAddrAddr(SmartPtr<TIPv6Addr> addr, long pref, long valid)
{
    if (pref>valid) {
	    //FIXME: log incorrect values
    }
    this->Prefered = pref;
    this->Valid = valid;
    //memcpy(this->Address,addr,16);
    this->Addr=addr;
    //this part changes address to plain - unnecessary here
    //it is done inside TIPv6Addr
    /*ostrstream plain(this->Plain,sizeof(this->Plain));
    for (int i=0; i<16;i++) {
        plain.fill('0');
        plain.width(2);
        plain  << hex << (int)(unsigned char)addr[i];
        if (i%2==1 && i!=15) plain << ":";
    }
    plain << ends;*/
    this->Timestamp = now();
    this->Tentative = DONTKNOWYET;
}

long TAddrAddr::getPref()
{
    return Prefered;
}

long TAddrAddr::getValid()
{
    return Valid;
}

SmartPtr<TIPv6Addr> TAddrAddr::get()
{
    return Addr;
}


// return Prefered time left
long TAddrAddr::getPrefTimeout()
{
    long ts = this->Timestamp + this->Prefered - now();
    if (ts>0) 
        return ts;
    else 
        return 0;
}

// return Valid time left
long TAddrAddr::getValidTimeout()
{
    long ts = (this->Timestamp) + (this->Valid) - now();
    if (ts>0) 
        return ts;
    else 
        return 0;
}

// return timestamp
long TAddrAddr::getTimestamp()
{
    return this->Timestamp;
}

// set timestamp
void TAddrAddr::setTimestamp(long ts)
{
    this->Timestamp = ts;
}

void TAddrAddr::setTentative(enum ETentative state)
{
    this->Tentative = state;
}

void TAddrAddr::setPref(long pref)
{
    this->Prefered = pref;
}

void TAddrAddr::setValid(long valid)
{
    this->Valid = valid;
}

// set timestamp
void TAddrAddr::setTimestamp()
{
    this->Timestamp = now();
}

enum ETentative TAddrAddr::getTentative()
{
    return Tentative;
}

ostream & operator<<(ostream & strum,TAddrAddr &x) {
    strum << "<AddrAddr"
	
	  << " timestamp=\"" << x.Timestamp << "\""
	  << " pref=\"" << x.Prefered <<"\""
	  << " valid=\"" << x.Valid <<  "\""
	  << ">" << x.Addr->getPlain()<< "\"</AddrAddr>" << std::endl;
    return strum;
}
