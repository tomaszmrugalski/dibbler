#include "SrvOptRapidCommit.h"

TSrvOptRapidCommit::TSrvOptRapidCommit( char * buf,  int n, TMsg* parent)
	:TOptRapidCommit(buf,n, parent)
{

}
TSrvOptRapidCommit::TSrvOptRapidCommit(TMsg* parent)
	:TOptRapidCommit(parent)
{
}

bool TSrvOptRapidCommit::doDuties()
{
    return true;
}
