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