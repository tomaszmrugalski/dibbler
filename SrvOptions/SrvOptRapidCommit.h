#ifndef SRVRAPIDCOMMIT_H_HEADER_INCLUDED_C112660F
#define SRVRAPIDCOMMIT_H_HEADER_INCLUDED_C112660F

#include "DHCPConst.h"
#include "OptRapidCommit.h"

class TSrvOptRapidCommit : public TOptRapidCommit
{
  public:
    TSrvOptRapidCommit(TMsg* parent);
    TSrvOptRapidCommit( char * buf,  int n, TMsg* parent);
	bool doDuties();
};


#endif /* RAPIDCOMMIT_H_HEADER_INCLUDED_C112660F */
