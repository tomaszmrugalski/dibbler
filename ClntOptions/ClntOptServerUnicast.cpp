#include "ClntOptServerUnicast.h"

TClntOptServerUnicast::TClntOptServerUnicast( char * buf,  int n, TMsg* parent)
	:TOptServerUnicast(buf,n, parent)
{

}
bool TClntOptServerUnicast::doDuties()
{
    return false;
}
