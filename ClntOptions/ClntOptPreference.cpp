#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "DHCPConst.h"
#include "ClntOptPreference.h"

TClntOptPreference::TClntOptPreference( char * buf,  int n, TMsg* parent)
	:TOptPreference(buf,n, parent)
{

}

TClntOptPreference::TClntOptPreference( char pref, TMsg* parent)
	:TOptPreference(pref, parent)
{
}

bool TClntOptPreference::doDuties()
{
    return false;
}
