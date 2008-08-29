/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef CLNTAUTHENTICATION_H_HEADER_INCLUDED
#define CLNTAUTHENTICATION_H_HEADER_INCLUDED

#include "DHCPConst.h"
#include "OptAuthentication.h"

class TClntOptAuthentication : public TOptAuthentication
{
  public:
    TClntOptAuthentication( char * buf,  int n, TMsg* parent);
    TClntOptAuthentication(TMsg* parent);
	bool doDuties();
};


#endif /* AUTHENTICATION_H_HEADER_INCLUDED */
