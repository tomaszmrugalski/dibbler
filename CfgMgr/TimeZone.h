#ifndef TIMEZONE_H
#define TIMEZONE_H

#include <iostream>
#include <string>

using namespace std;

class TTimeZone
{
public:
    TTimeZone(string timeZone);    
    bool isValid();
    int getLength();
    string get();
private:
    string ValidateZoneName(const char* &start);
    string ValidateTime(const char* &start); //hh[:mm[:ss]]
    string ValidateOffset(const char* &start); //[+|-]hh[:mm[:ss]]
    string ValidateDayOfTheYear(const char* &start); //Jn,n,Mm.n.d
    string ValidateJulianDay(const char* &start); //n
    string ValidateZeroJulianDay(const char* &start); //n
    string ValidateDayOfTheWeek(const char* &start); //m.n.d
    string readNumber(const char* &start,int &number);
    string Std;
    string StdOffset;
    string Dst;
    string DstOffset;
    string Start;
    string StartTime;
    string End;
    string EndTime;
    bool   Valid;
    string TZone;
};
#endif
