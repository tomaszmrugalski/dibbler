#ifndef CLNTSTATUSCODE_H_HEADER_INCLUDED_C11257D8
#define CLNTSTATUSCODE_H_HEADER_INCLUDED_C11257D8


#include "OptStatusCode.h"
#include "DHCPConst.h"

class TClntOptStatusCode : public TOptStatusCode 
{
  public:
    TClntOptStatusCode( char * buf,  int len, TMsg* parent);
};

#endif /* STATUSCODE_H_HEADER_INCLUDED_C11257D8 */
