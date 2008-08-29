/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef SRVUSERCLASS_H_HEADER_INCLUDED_C11211AD
#define SRVUSERCLASS_H_HEADER_INCLUDED_C11211AD

#include "OptUserClass.h"
#include "DHCPConst.h"

class TSrvOptUserClass : public TOptUserClass 
{
 public:
    TSrvOptUserClass( char * buf,  int n, TMsg* parent);
    bool doDuties();
};



#endif 
