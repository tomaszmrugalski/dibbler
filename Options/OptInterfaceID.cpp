#include "OptInterfaceID.h"
#include "DHCPConst.h"

TOptInterfaceID::TOptInterfaceID( char * &buf,  int &n, TMsg* parent)
	:TOpt(OPTION_INTERFACE_ID, parent)
{

}

//##ModelId=3EFF083B03B9
 int TOptInterfaceID::getSize()
{
	return 0;
}

//##ModelId=3EFF0840004E
 char * TOptInterfaceID::storeSelf( char *)
{
    // FIXME
    return NULL;
}
