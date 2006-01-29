#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "DHCPConst.h"
#include "SrvOptServerIdentifier.h"

TSrvOptServerIdentifier::TSrvOptServerIdentifier( SmartPtr<TDUID> duid, TMsg* parent)
	:TOptDUID(duid, parent)
{
}

TSrvOptServerIdentifier::TSrvOptServerIdentifier(char* buf, int bufsize, TMsg* parent)
    :TOptDUID(buf,bufsize, parent)
{
}

bool TSrvOptServerIdentifier::doDuties()
{
    return true;
}
