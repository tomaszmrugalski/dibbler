#ifndef SRVELAPSED_H_HEADER_INCLUDED_C112034D
#define SRVELAPSED_H_HEADER_INCLUDED_C112034D

#include "OptElapsed.h"

//##ModelId=3EC75CE303E5
class TSrvOptElapsed : public TOptElapsed
{
  public:
    TSrvOptElapsed(TMsg* parent);
    TSrvOptElapsed( char * buf,  int n, TMsg* parent);
	bool doDuties();
};



#endif /* ELAPSED_H_HEADER_INCLUDED_C112034D */
