#include "SrvOptServerUnicast.h"

TSrvOptServerUnicast::TSrvOptServerUnicast( char * buf,  int n, TMsg* parent)
	:TOptServerUnicast(buf,n, parent)
{

}
bool TSrvOptServerUnicast::doDuties()
{
    return true;
}
