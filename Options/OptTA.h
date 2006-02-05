/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: OptTA.h,v 1.1.2.1 2006-02-05 23:39:52 thomson Exp $
 */

class TOptTA;
#ifndef OPTIA_TA_H
#define OPTIA_TA_H

#include "Opt.h"

// length without optType and Length
#define OPTION_IA_TA_LEN 4

class TOptTA : public TOpt
{
  public:
    TOptTA( long IAID, TMsg* parent);
    TOptTA( char * &buf, int &bufsize, TMsg* parent);
    int getSize();
    int getStatusCode();
    
    unsigned long getIAID();
    unsigned long getMaxValid();
    int countAddrs();
    
    char * storeSelf( char* buf);
    bool isValid();
 protected:
    bool Valid;
    unsigned long IAID;
};


#endif /* OPTIA_TA_H */

/*
 * $Log: not supported by cvs2svn $
 */
