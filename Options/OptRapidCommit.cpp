#include <stdlib.h>
#ifdef LINUX
#include <netinet/in.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#endif

#include "OptRapidCommit.h"

TOptRapidCommit::TOptRapidCommit( char * &buf,  int &n, TMsg* parent)
	:TOpt(OPTION_RAPID_COMMIT, parent)
{
	//Here are read common things for server and client
}

TOptRapidCommit::TOptRapidCommit(TMsg* parent)
	:TOpt(OPTION_RAPID_COMMIT, parent)
{

}

//##ModelId=3ECE0A2C03D3
 int TOptRapidCommit::getSize()
{
	return 4;
}

//##ModelId=3ECE0A2D0028
 char * TOptRapidCommit::storeSelf( char* buf)
{
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(getSize()-4);
    return buf+2;
}
