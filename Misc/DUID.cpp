/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: DUID.cpp,v 1.13 2008-08-29 00:07:30 thomson Exp $
 */

#include <iostream>
#include <sstream>
#include <string.h>

#include "DUID.h"
#include "Logger.h"

TDUID::TDUID()
{
    DUID=0;
    len=0;
    Plain="";
}

// packed
TDUID::TDUID(const char* DUID,int DUIDlen)
{
    this->DUID = 0;

    if ((DUID)&&(DUIDlen))
    {
        this->DUID=new char[DUIDlen];
        memcpy(this->DUID,DUID,DUIDlen);
        this->len=DUIDlen;
	this->packedToPlain();
    }
    else
    {
        this->DUID=NULL;
        this->len=0;
	this->Plain="";
    }
}

void TDUID::packedToPlain() {
    ostringstream tmp;
    for(int i=0; i<this->len; i++) {
	if (i) tmp << ":";
	tmp << setfill('0')<<setw(2)<<hex<< (unsigned int) this->DUID[i];
    }
    tmp << dec;
    this->Plain = tmp.str();
 }

void TDUID::plainToPacked() {
    int DUIDlen = Plain.length();
    char * tmp = new char[DUIDlen>>1];
    unsigned char digit;
    int i=0, j=0;
    bool twonibbles = false;
    while (i<DUIDlen)
    {
	if (Plain[i]==':') {
	    i++;
	}
	digit = Plain[i];
	if (isalpha(digit))
	    digit=toupper(digit)-'A'+10;
	else
	    digit-='0';
	tmp[j]<<=4;
	tmp[j]|=digit;
	i++;
	if (twonibbles) {
	    twonibbles = false;
	    j++;
	} else 
	    twonibbles = true;
    }

    DUID = new char[j];
    memmove(DUID, tmp, j);
    delete [] tmp;
    this->len = j;
}

// plain
TDUID::TDUID(const char* Plain)
{
    if (!Plain) {
	this->DUID=NULL;
	this->len=0;
	return;
    }

    this->Plain = Plain;

    plainToPacked();
}

TDUID::~TDUID() {
    if (this->len)
	delete [] this->DUID;
}

TDUID::TDUID(const TDUID &duid) {
    this->DUID=new char[duid.len];
    memcpy(this->DUID,duid.DUID,duid.len);
    this->len=duid.len;
    this->Plain = duid.Plain;
}

TDUID& TDUID::operator=(const TDUID &duid) {
    if (this==&duid)
        return *this;
    
    if(this->DUID)
        delete this->DUID;
    
    this->DUID=new char [duid.len];
    memcpy(this->DUID,duid.DUID,duid.len);
    this->len=duid.len;
    this->Plain=duid.Plain;
    
    return *this;
}

bool TDUID::operator==(const TDUID &duid) {
    if (this->len!=duid.len)
        return false;
    else
        return !memcmp(this->DUID,duid.DUID,this->len);
}

bool TDUID::operator<=(const TDUID &duid) {
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
            if(this->DUID[this->len-i]>duid.DUID[duid.len-i]) {
                return true; //right is smaller
            } else {
                if(this->DUID[this->len-i]<duid.DUID[duid.len-i])
                    return false;	//left is smaller
	    }
        };
    return false;//left is equal to right
}

int TDUID::getLen() {
    return this->len;
}

const string TDUID::getPlain() {
    return this->Plain;
}

char * TDUID::storeSelf(char* buf) {
    memcpy(buf,DUID,len);
    return buf+len;
}

ostream& operator<<(ostream& out,TDUID&  duid) {
    if ( (duid.DUID && duid.len) ) {
	out << "<duid length=\"" << duid.len << "\">"
	    << duid.Plain << "</duid>" << std::endl;
    } else {
        out << "<duid length=\"0\"></duid>" << std::endl;
    }
    return out;
}
