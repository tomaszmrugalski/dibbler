#ifndef SRVSERVERUNICAST_H_HEADER_INCLUDED_C1126AA1
#define SRVTSERVERUNICAST_H_HEADER_INCLUDED_C1126AA1

#include "Opt.h"
#include "DHCPConst.h"
#include "OptServerUnicast.h"

class TSrvOptServerUnicast : public TOptServerUnicast 
{
public:
    TSrvOptServerUnicast( char * buf,  int n, TMsg* parent);
	bool doDuties();	
};



#endif /* SERVERUNICAST_H_HEADER_INCLUDED_C1126AA1 */
