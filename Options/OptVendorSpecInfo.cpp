#include "OptVendorSpecInfo.h"
#include "DHCPConst.h"

TOptVendorSpecInfo::TOptVendorSpecInfo( char * &buf,  int &n, TMsg* parent)
	:TOpt(OPTION_VENDOR_OPTS, parent)
{

}


 int TOptVendorSpecInfo::getSize()
{
	return 0;
}



 char * TOptVendorSpecInfo::storeSelf( char* buf)
{
	return 0;
}

bool TOptVendorSpecInfo::isValid()
{
    return true;
}
