#include "ClntOptUserClass.h"

TClntOptUserClass::TClntOptUserClass( char * buf,  int n, TMsg* parent)
	:TOptUserClass(buf,n, parent)
{

}
bool TClntOptUserClass::doDuties()
{
    return false;
}
