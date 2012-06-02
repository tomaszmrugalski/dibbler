/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef CLNTAAAAUTHENTICATION_H
#define CLNTAAAAUTHENTICATION_H

#include "DHCPConst.h"
#include "OptAAAAuthentication.h"
#include "ClntMsg.h"

class TClntOptAAAAuthentication : public TOptAAAAuthentication
{
  public:
    TClntOptAAAAuthentication( char * buf,  int n, TMsg* parent);
    TClntOptAAAAuthentication(TClntMsg* parent);
    bool doDuties();
};


#endif /* AAAAUTHENTICATION_H_HEADER_INCLUDED */
