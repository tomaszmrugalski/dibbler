#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "DHCPConst.h"
#include "ClntOptServerIdentifier.h"


TClntOptServerIdentifier::TClntOptServerIdentifier( char * duid,  int n, TMsg* parent)
	:TOptServerIdentifier(duid,n, parent)
{
}
TClntOptServerIdentifier::TClntOptServerIdentifier(SmartPtr<TDUID> duid, TMsg* parent)
    :TOptServerIdentifier(duid, parent)
{

}
bool TClntOptServerIdentifier::doDuties()
{
    return false;
}
