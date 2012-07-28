/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef CLNTUSERCLASS_H_HEADER_INCLUDED_C11211AD
#define CLNTUSERCLASS_H_HEADER_INCLUDED_C11211AD

#include "OptUserClass.h"
#include "DHCPConst.h"

//##ModelId=3EC75CE30350
class TClntOptUserClass : public TOptUserClass 
{
  public:
    TClntOptUserClass( char * buf,  int n, TMsg* parent);

	bool doDuties();
};



#endif /* USERCLASS_H_HEADER_INCLUDED_C11211AD */
