/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "DHCPConst.h"
#include "ClntOptPreference.h"

TClntOptPreference::TClntOptPreference( char * buf,  int n, TMsg* parent)
  :TOptInteger(OPTION_PREFERENCE, 1, buf,n, parent)
{

}

TClntOptPreference::TClntOptPreference( char pref, TMsg* parent)
  :TOptInteger(OPTION_PREFERENCE, 1, pref, parent)
{
}

bool TClntOptPreference::doDuties()
{
    return true;
}
