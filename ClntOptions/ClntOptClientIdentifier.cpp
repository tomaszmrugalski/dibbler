#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "DHCPConst.h"
#include "ClntOptClientIdentifier.h"


TClntOptClientIdentifier::TClntOptClientIdentifier(char * duid, int n, TMsg* parent)
    :TOptClientIdentifier(duid,n, parent)
{
}

TClntOptClientIdentifier::TClntOptClientIdentifier(SmartPtr<TDUID> duid, TMsg* parent)
    :TOptClientIdentifier(duid, parent)
{
}

bool TClntOptClientIdentifier::doDuties()
{
    return false;
}
