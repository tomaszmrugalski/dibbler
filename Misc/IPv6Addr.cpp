#include <stdlib.h>
#include <string.h>
#include "IPv6Addr.h"
#include "Portable.h"

TIPv6Addr::TIPv6Addr()
{
    memset(Addr,0,16);
    inet_ntop6(Addr,Plain);
}
TIPv6Addr::TIPv6Addr(char* addr, bool plain)
{
    if (plain)
    {
        strcpy(Plain,addr);
        inet_pton6(Plain,Addr);
    }
    else
    {
        memcpy(Addr,addr,16);
        inet_ntop6(Addr,Plain);
    }
}

char* TIPv6Addr::getAddr()
{
    return Addr;
}

char* TIPv6Addr::getPlain()
{
    inet_ntop6( Addr, Plain);
    return Plain;
}

void TIPv6Addr::setAddr(char* addr)
{
    memcpy(Addr,addr,16);
    inet_ntop6(Addr,Plain);
}

char* TIPv6Addr::storeSelf(char *buf)
{
    memcpy(buf,this->Addr,16);
    return buf+16;
}


bool TIPv6Addr::operator==(const TIPv6Addr &other)
{
    return !memcmp(this->Addr,other.Addr,16);
}

ostream& operator<<(ostream& out,TIPv6Addr& addr)
{
	char buf[48];
	if (addr.Addr) {
		inet_ntop6(addr.Addr, buf);
		out << buf;
	} else {
		out << "::";
	}
    return out;
}

bool TIPv6Addr::operator<=(const TIPv6Addr &other)
{
    
	for (int i=0;i<16;i++)
    {
        if (Addr[i]<other.Addr[i])
            return true;
        if(Addr[i]>other.Addr[i])
			return false;
    }
	return true; //hmm: are they equal
}

TIPv6Addr TIPv6Addr::operator-(const TIPv6Addr &other)
{   
    char result[16];
    memset(result,0,16);
	char carry=0;
    for (int i=15;i>=0;i--)
    {
        unsigned int left=Addr[i];
        unsigned int right=other.Addr[i];
        if(left>=(right+carry))
        {
            result[i]=left-other.Addr[i]-carry;
            carry=0;
        }
        else
        {
            result[i]=Addr[i]+256-other.Addr[i]-carry;
            carry=1;
        }
    }
    return TIPv6Addr(result);
}

TIPv6Addr TIPv6Addr::operator+(const TIPv6Addr &other)
{   
    char result[16];
    memset(result,0,16);
    unsigned int carry=0;
    for (int i=15;i>=0;i--)
    {
        unsigned int left=Addr[i];
        unsigned int right=other.Addr[i];
        if(left+right+carry>255)
        {
            result[i]=char (left+right+carry-256);
            carry=1;
        }
        else
        {
            result[i]=left+right+carry;
            carry=0;
        }
    }
    return TIPv6Addr(result);
}
//Decreases randomly an address
TIPv6Addr& TIPv6Addr::operator--()
{
    bool freely=false;
    for (int i=15;i>=0;i--)
    {
        int j=i-1;
        while((j>=0)&&(!Addr[j])) j--;
        int r;
        if (j>0) r=rand()%256;
        else r=rand()%(Addr[i]+1);
        if (r>Addr[i])
        {
            Addr[j]--;
            for(j++;j<i;j++) Addr[j]=255;
            Addr[i]=char(int(256)+int(Addr[i])-r);
        }
        else Addr[i]=r;
    }
    return *this;
}
