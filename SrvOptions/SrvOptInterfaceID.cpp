#include "SrvOptInterfaceID.h"

TSrvOptInterfaceID::TSrvOptInterfaceID( char * buf,  int n, TMsg* parent)
	:TOptInterfaceID(buf,n, parent)
{

}
bool TSrvOptInterfaceID::doDuties()
{
    return true;
}
