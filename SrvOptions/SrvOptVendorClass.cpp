#include "SrvOptVendorClass.h"

TSrvOptVendorClass::TSrvOptVendorClass( char * buf,  int n, TMsg* parent)
	:TOptVendorClass(buf,n, parent)
{
	
}
bool TSrvOptVendorClass::doDuties()
{
    return true;
}
