#ifndef CLNTSERVERUNICAST_H_HEADER_INCLUDED_C1126AA1
#define CLNTSERVERUNICAST_H_HEADER_INCLUDED_C1126AA1

#include "Opt.h"
#include "DHCPConst.h"
#include "OptServerUnicast.h"

//##ModelId=3EC75CE3038C
class TClntOptServerUnicast : public TOptServerUnicast 
{
  public:
    TClntOptServerUnicast( char * buf,  int n, TMsg* parent);
    bool doDuties();
};



#endif /* SERVERUNICAST_H_HEADER_INCLUDED_C1126AA1 */
