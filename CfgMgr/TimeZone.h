/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef TIMEZONE_H
#define TIMEZONE_H

#include <cstdlib>
#include <iostream>
#include <string>

class TTimeZone
{
public:
    TTimeZone(const std::string timeZone);
    bool isValid();
    int getLength();
    std::string get();
private:
    std::string ValidateZoneName(const char* &start);
    std::string ValidateTime(const char* &start); //hh[:mm[:ss]]
    std::string ValidateOffset(const char* &start); //[+|-]hh[:mm[:ss]]
    std::string ValidateDayOfTheYear(const char* &start); //Jn,n,Mm.n.d
    std::string ValidateJulianDay(const char* &start); //n
    std::string ValidateZeroJulianDay(const char* &start); //n
    std::string ValidateDayOfTheWeek(const char* &start); //m.n.d
    std::string readNumber(const char* &start,int &number);
    std::string Std;
    std::string StdOffset;
    std::string Dst;
    std::string DstOffset;
    std::string Start;
    std::string StartTime;
    std::string End;
    std::string EndTime;
    bool   Valid;
    std::string TZone;
};
#endif
