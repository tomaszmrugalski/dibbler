#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "SmartPtr.h"
#include "SrvCfgMgr.h"
#include "SrvOptOptionRequest.h"


TSrvOptOptionRequest::TSrvOptOptionRequest( char * buf,  int n, TMsg* parent)
	:TOptOptionRequest(buf,n, parent)
{
}

TSrvOptOptionRequest::TSrvOptOptionRequest(TMsg* parent)
    :TOptOptionRequest(parent)
{

}

bool TSrvOptOptionRequest::doDuties()
{
    return true;
}
