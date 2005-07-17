#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "DHCPConst.h"
#include "SrvOptServerIdentifier.h"

//##ModelId=3EFF0A8F00FA
TSrvOptServerIdentifier::TSrvOptServerIdentifier( SmartPtr<TDUID> duid, TMsg* parent)
	:TOptServerIdentifier(duid, parent)
{
}

TSrvOptServerIdentifier::TSrvOptServerIdentifier(char* buf, int bufsize, TMsg* parent)
    :TOptServerIdentifier(buf,bufsize, parent)
{
}

bool TSrvOptServerIdentifier::doDuties()
{
    return true;
}
