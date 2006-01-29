#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#include "DHCPConst.h"
#include "SrvOptClientIdentifier.h"

TSrvOptClientIdentifier::TSrvOptClientIdentifier(
    SmartPtr<TDUID> duid, TMsg* parent)
    :TOptDUID(duid, parent)
{
}

TSrvOptClientIdentifier::TSrvOptClientIdentifier(char* buf, int bufsize, TMsg* parent)
    :TOptDUID(buf,bufsize, parent)
{
}

bool TSrvOptClientIdentifier::doDuties()
{
    return true;
}
