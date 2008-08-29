/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "TimeZone.h"


TTimeZone::TTimeZone(string timeZone)
{
    Valid=true;
    const char *ptr=timeZone.c_str();
    TZone=timeZone;
    Std=ValidateZoneName(ptr);
    if (!(Valid&&(*ptr))) return;
    
    StdOffset=ValidateOffset(ptr);
    if (!(Valid&&(*ptr))) return;
    
    Dst=ValidateZoneName(ptr);
    if (!(Valid&&(*ptr))) return;

    if(*ptr!=',') 
    {
        DstOffset=ValidateOffset(ptr);
        if (!(Valid&&(*ptr))) return;
    }
    if(*ptr!=',') {Valid=false; return;};
    ptr++;
    if(*ptr!=',')
    {
        Start=ValidateDayOfTheYear(ptr);
        if (!Valid) return;
        if (*ptr=='/')
        {
            StartTime=ValidateTime((++ptr));
            if (!Valid) return;
        }
    }
    if (*ptr!=',') {Valid=false; return;};
    ptr++;

    End=ValidateDayOfTheYear(ptr);
    if (!Valid) return;
    if (*ptr=='/')
    {
        EndTime=ValidateTime(++ptr);
    }
    if (*ptr) {Valid=false; return;}
    if (!Valid) return;
    TZone=timeZone;
} 

bool TTimeZone::isValid()
{
    return this->Valid;
} 


string TTimeZone::ValidateZoneName(const char* &start)  
{ 
    string zoneName="";
    Valid=true;
    if (*start==':')
    {
        Valid=false;
        return zoneName;
    }
    while(!( isdigit(*start)||(*start==',')||
             (*start=='+')||(*start=='-')||(!*start)
         ) )
    {
        zoneName=zoneName+*start;
        start++;
    }
    if (zoneName.length()<3) {Valid=false; return "";};
    return zoneName;
} 

string TTimeZone::ValidateTime(const char* &start)  //hh[:mm[:ss]] 
{ 
    Valid=true;
    string strtime="";
    int hh=24;
    
    strtime=readNumber(start,hh);
    if ((!Valid)||(hh>23)) {Valid=false; return "";};    
    if (*start!=':') return strtime;
    
    int mm=60;
    strtime+=":"+readNumber((++start),mm);
    if ((!Valid)||(mm>59)) {Valid=false; return "";};
    if (*start!=':') return strtime;
    
    int ss=60;
    strtime+=":"+readNumber((++start),ss);
    if ((!Valid)||(ss>59)) {Valid=false; return "";};
    
    return strtime;
} 

string TTimeZone::ValidateOffset(const char* &start)  //[+|-]hh[:mm[:ss]] 
{ 
    Valid=true;
    string strtime="";
    if ((*start=='+')||(*start=='-')) {
        strtime+=*start; 
        start++; 
    }
    strtime+=ValidateTime(start);
    if (!Valid) return "";
    return strtime;
} 

string TTimeZone::ValidateDayOfTheYear(const char* &start)  //Jn,n,Mm.n.d 
{ 
    string str="";
    switch(*start)
    {
    case 'J':
        str='J'+ValidateJulianDay((++start));
        break;
    case 'M':
        str='M'+ValidateDayOfTheWeek(++start);
        break;
    default:
        str=ValidateZeroJulianDay((start));
        break;
    }
    if (!Valid) return "";
    return str;
} 

string TTimeZone::ValidateJulianDay(const char* &start)  //n 
{ 
    int n=0;
    string str=readNumber(start, n);
    if ((!Valid)||(!n)||(n>365)) {Valid=false; return "";};
    return str;
}   

string TTimeZone::ValidateZeroJulianDay(const char* &start)  //n 
{ 
    int n=0;
    string str=readNumber(start, n);
    if ((!Valid)||(n>365)) {Valid=false; return "";};
    return str;
} 

string TTimeZone::ValidateDayOfTheWeek(const char* &start)  
{ 
    int m=0;
    string str=readNumber(start,m);
    if ((!Valid)||(m>12)||(m<1)||(*start!='.')) { Valid=false; return "";}
    
    int n=0;
    str+="."+readNumber(start,n);
    if ((!Valid)||(n>5)||(n<1)||(*start!='.')) { Valid=false; return "";}
    
    int d=0;
    str+="."+readNumber(start,d);
    if ((!Valid)||(d>5)||(d<1)) { Valid=false; return "";}
    
    return str;
} 

string TTimeZone::readNumber(const char* &start,int &number)
{

    string retVal="";
    int len=0;
    while(isdigit(*start))
    {
        retVal+=*start;
        len++;
        if (len>3) {Valid=false; return "";};
        start++;
    }
    if (!len) {Valid=false; return "";};
    number=atoi(retVal.c_str());
    return retVal;
}

int TTimeZone::getLength()
{
    if (!Valid)
        return 0;
    else
        return  (int)TZone.length();
}

string TTimeZone::get()
{
    if (!Valid) 
        return "";
    else
        return TZone;
}
