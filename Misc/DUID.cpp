#include <iostream>

#include "DUID.h"
#include "Logger.h"

TDUID::TDUID()
{
    DUID=NULL;
    len=0;
}

// packed
TDUID::TDUID(char* DUID,int DUIDlen)
{
    if ((DUID)&&(DUIDlen))
    {
        this->DUID=new char[DUIDlen];
        memcpy(this->DUID,DUID,DUIDlen);
        this->len=DUIDlen;
    }
    else
    {
        this->DUID=NULL;
        this->len=0;
    }
}

// plain
TDUID::TDUID(char* Plain)
{
    if (!Plain) {
	this->DUID=NULL;
	this->len=0;
	return;
    }

    int DUIDlen = strlen((char*)Plain);
    this->DUID = new char[DUIDlen>>1];
    unsigned char digit;
    for (int i=0;i<DUIDlen; i++)	
    {
	digit = Plain[i];
	if (isalpha(digit))
	    digit=toupper(digit)-'A'+10;
	else
	    digit-='0';
	DUID[i>>1]<<=4;
	DUID[i>>1]|=digit;
    }
    DUIDlen>>=1;
    this->len = DUIDlen;
}

TDUID::~TDUID()
{
    if (this->len)
	delete this->DUID;
}

TDUID::TDUID(const TDUID &duid)
{
    this->DUID=new char [duid.len];
    memcpy(this->DUID,duid.DUID,duid.len);
    this->len=duid.len;
}

TDUID& TDUID::operator=(const TDUID &duid)
{
    if (this==&duid)
        return *this;
    
    if(this->DUID)
        delete this->DUID;
    
    this->DUID=new char [duid.len];
    memcpy(this->DUID,duid.DUID,duid.len);
    this->len=duid.len;

    return *this;
}

bool TDUID::operator==(const TDUID &duid)
{
    if (this->len!=duid.len)
        return false;
    else
        return !memcmp(this->DUID,duid.DUID,this->len);
}

bool TDUID::operator<=(const TDUID &duid)
{
    
    int minLen=this->len<duid.len?this->len:duid.len;
    int maxLen=this->len>=duid.len?this->len:duid.len;
    
    bool retVal=false;
    
    const char *longer; //,*shorter;	
    if (this->len<duid.len)
        longer=duid.DUID;
    else
    {
        longer=this->DUID;
        retVal=true;
    };

    //if they are not equal check if longer has only zeros at the begining
    for (int i=0;i<(maxLen-minLen);i++)
        if (longer[i])
            return retVal;
    //if they are equal sizes or longer has at the beginning only zeroes
    for (int i=minLen-1;i>=0;i--)
        if(this->DUID[this->len-i]!=duid.DUID[duid.len-i])
        {
            if(this->DUID[this->len-i]>duid.DUID[duid.len-i])
                return true; //right is smaller
            else
                if(this->DUID[this->len-i]<duid.DUID[duid.len-i])
                    return false;	//left is smaller
        };
    return false;//left is equal to right
}

int TDUID::getLen()
{
    return this->len;
}

char * TDUID::storeSelf(char* buf)
{
    memcpy(buf,DUID,len);
    return buf+len;
}

ostream& operator<<(ostream& out,TDUID&  duid)
{
    if ( (duid.DUID && duid.len) )
    {
		out << "<duid length=\"" << duid.len << "\">";
        for(int i=0;i<duid.len;i++)
            out<<setfill('0')<<setw(2)<<hex<< (unsigned int)duid.DUID[i];
		out << dec << "</duid>" << std::endl;
		
    }
	else {
        out << "<duid length=\"0\"></duid>" << std::endl;
	}
	return out;
}
