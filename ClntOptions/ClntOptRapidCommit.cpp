#include "ClntOptRapidCommit.h"


TClntOptRapidCommit::TClntOptRapidCommit( char * buf,  int n, TMsg* parent)
	:TOptRapidCommit(buf,n, parent)
{

}

TClntOptRapidCommit::TClntOptRapidCommit(TMsg* parent)
    :TOptRapidCommit(parent)
{
}

bool TClntOptRapidCommit::doDuties()
{
    return false;
}
