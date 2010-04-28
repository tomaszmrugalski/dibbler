/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef SRVPREFERENCE_H_HEADER
#define SRVPREFERENCE_H_HEADER

#include "DHCPConst.h"
#include "OptInteger.h"

class TSrvOptPreference : public TOptInteger 
{
  public:
    TSrvOptPreference( char * buf,  int n, TMsg* parent);
    TSrvOptPreference( char pref, TMsg* parent);

    bool doDuties();
};



#endif /* PREFERENCE_H_HEADER_INCLUDED_C1126A41 */
