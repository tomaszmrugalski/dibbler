#include <string>
#include "SrvOptStatusCode.h"

TSrvOptStatusCode::TSrvOptStatusCode( char * buf,  int len, TMsg* parent)
	:TOptStatusCode(buf,len, parent)
{

}

TSrvOptStatusCode::TSrvOptStatusCode(int status,string message, TMsg* parent)
	:TOptStatusCode(status,message, parent)
{
//	this->StatusCode = status;
//   this->Message = message;
}

bool TSrvOptStatusCode::doDuties()
{
    return true;
}
