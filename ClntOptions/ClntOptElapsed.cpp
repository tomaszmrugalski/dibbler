#include <stdlib.h>
#ifdef LINUX
#include <netinet/in.h>
#endif

#include <time.h>
#include "DHCPConst.h"
#include "Portable.h"
#include "ClntOptElapsed.h"


TClntOptElapsed::TClntOptElapsed( char * buf,  int n, TMsg* parent)
	:TOptElapsed(buf,n, parent)
{

}

TClntOptElapsed::TClntOptElapsed(TMsg* parent)
	:TOptElapsed(parent)
{

}

bool TClntOptElapsed::doDuties()
{

    return false;
}
