#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#include "DHCPConst.h"
#include "SrvOptClientIdentifier.h"

//##ModelId=3EFF0A8E0399
TSrvOptClientIdentifier::TSrvOptClientIdentifier(
	 SmartPtr<TDUID> duid, TMsg* parent)
	:TOptClientIdentifier(duid, parent)
{
}

TSrvOptClientIdentifier::TSrvOptClientIdentifier(char* buf, int bufsize, TMsg* parent)
    :TOptClientIdentifier(buf,bufsize, parent)
{
}

bool TSrvOptClientIdentifier::doDuties()
{
    return true;
}
