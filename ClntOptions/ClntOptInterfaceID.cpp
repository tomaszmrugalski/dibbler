#include "ClntOptInterfaceID.h"

TClntOptInterfaceID::TClntOptInterfaceID( char * buf,  int n, TMsg* parent)
	:TOptInterfaceID(buf,n, parent)
{

}
bool TClntOptInterfaceID::doDuties()
{
    return false;
}
