#include "ClntOptVendorClass.h"

TClntOptVendorClass::TClntOptVendorClass( char * buf,  int n, TMsg* parent)
	:TOptVendorClass(buf,n, parent)
{
	
}
bool TClntOptVendorClass::doDuties()
{
    return false;
}
