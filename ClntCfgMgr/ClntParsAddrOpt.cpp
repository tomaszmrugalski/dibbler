/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#include <limits.h>
#include "ClntParsAddrOpt.h"

TClntParsAddrOpt::TClntParsAddrOpt()
{
    Pref=ULONG_MAX;
    PrefSendOpt=Send;
    Valid=ULONG_MAX;
    ValidSendOpt=Send;
}

long TClntParsAddrOpt::getPref()
{
    return Pref;
}

void TClntParsAddrOpt::setPref(long pref)
{
    Pref=pref;
}

ESendOpt TClntParsAddrOpt::getPrefSendOpt()
{
    return PrefSendOpt;
}

void TClntParsAddrOpt::setPrefSendOpt(ESendOpt opt)
{
    PrefSendOpt=opt;
}

long TClntParsAddrOpt::getValid()
{
    return Valid;
}

void TClntParsAddrOpt::setValid(long valid)
{
    Valid=valid;
}

ESendOpt TClntParsAddrOpt::getValidSendOpt()
{
    return ValidSendOpt;
}

void TClntParsAddrOpt::setValidSendOpt(ESendOpt opt)
{
    ValidSendOpt=opt;
}
