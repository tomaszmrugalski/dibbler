#ifndef CLNTRAPIDCOMMIT_H_HEADER_INCLUDED_C112660F
#define CLNTRAPIDCOMMIT_H_HEADER_INCLUDED_C112660F

#include "DHCPConst.h"
#include "OptRapidCommit.h"

class TClntOptRapidCommit : public TOptRapidCommit
{
  public:
    TClntOptRapidCommit(TMsg* parent);
    TClntOptRapidCommit( char * buf,  int n, TMsg* parent);
	bool doDuties();
};


#endif /* RAPIDCOMMIT_H_HEADER_INCLUDED_C112660F */
