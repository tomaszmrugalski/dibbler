/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

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
