#include <stdlib.h>
#ifdef LINUX
#include <netinet/in.h>
#endif

#include <time.h>
#include "DHCPConst.h"
#include "Portable.h"
#include "SrvOptElapsed.h"


TSrvOptElapsed::TSrvOptElapsed( char * buf,  int n, TMsg* parent)
	:TOptElapsed(buf,n, parent)
{

}

TSrvOptElapsed::TSrvOptElapsed(TMsg* parent)
	:TOptElapsed(parent)
{

}

bool TSrvOptElapsed::doDuties()
{
    return true;
}
