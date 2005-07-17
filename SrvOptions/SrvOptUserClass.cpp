#include "SrvOptUserClass.h"

TSrvOptUserClass::TSrvOptUserClass( char * buf,  int n, TMsg* parent)
	:TOptUserClass(buf,n, parent)
{

}
bool TSrvOptUserClass::doDuties()
{
    return true;
}
