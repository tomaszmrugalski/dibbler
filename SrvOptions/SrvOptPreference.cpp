#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "DHCPConst.h"
#include "SrvOptPreference.h"

TSrvOptPreference::TSrvOptPreference( char * buf,  int n, TMsg* parent)
	:TOptPreference(buf,n, parent)
{

}

TSrvOptPreference::TSrvOptPreference( char pref, TMsg* parent)
	:TOptPreference(pref, parent)
{
}

bool TSrvOptPreference::doDuties()
{
    return true;
}
