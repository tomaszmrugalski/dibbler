#include "OptVendorClass.h"

TOptVendorClass::TOptVendorClass( char * &buf,  int &n, TMsg* parent)
	:TOpt(OPTION_VENDOR_CLASS, parent)
{
	
}


 int TOptVendorClass::getSize()
{
	return 0;
}



 char * TOptVendorClass::storeSelf( char* buf)
{
	return 0;
}

bool TOptVendorClass::isValid()
{
    return true;
}
