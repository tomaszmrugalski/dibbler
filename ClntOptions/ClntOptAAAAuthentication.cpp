/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 */
#include "ClntOptAAAAuthentication.h"

TClntOptAAAAuthentication::TClntOptAAAAuthentication( char * buf,  int n, TMsg* parent)
    :TOptAAAAuthentication(buf, n, parent) {
}

TClntOptAAAAuthentication::TClntOptAAAAuthentication(TClntMsg* parent)
    :TOptAAAAuthentication(parent) {
    setAAASPI(ClntCfgMgr().getAAASPI());
}

bool TClntOptAAAAuthentication::doDuties() {
    return false;
}
