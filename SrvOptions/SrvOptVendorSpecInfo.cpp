#include "SrvOptVendorSpecInfo.h"

TSrvOptVendorSpecInfo::TSrvOptVendorSpecInfo( char * buf,  int n, TMsg* parent)
	:TOptVendorSpecInfo(buf,n, parent)
{

}

bool TSrvOptVendorSpecInfo::doDuties()
{
    return true;
}
