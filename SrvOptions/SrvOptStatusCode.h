#ifndef SRVSTATUSCODE_H_HEADER_INCLUDED_C11257D8
#define SRVSTATUSCODE_H_HEADER_INCLUDED_C11257D8


#include "OptStatusCode.h"
#include "DHCPConst.h"

class TSrvOptStatusCode : public TOptStatusCode 
{
  public:
    TSrvOptStatusCode( char * buf,  int len, TMsg* parent);
    TSrvOptStatusCode(int status, string Text, TMsg* parent);
    
    bool doDuties();
};

#endif /* STATUSCODE_H_HEADER_INCLUDED_C11257D8 */
