#include "OptUserClass.h"

TOptUserClass::TOptUserClass( char * &buf,  int &n, TMsg* parent)
	:TOpt(OPTION_USER_CLASS, parent)
{

}

 int TOptUserClass::getSize()
{
    return 0;
}

 char * TOptUserClass::storeSelf( char* buf)
{
    return 0;
}
bool TOptUserClass::isValid()
{
    return true;
}
