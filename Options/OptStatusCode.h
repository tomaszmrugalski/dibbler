#ifndef STATUSCODE_H_HEADER_INCLUDED_C11257D8
#define STATUSCODE_H_HEADER_INCLUDED_C11257D8

#include <string>
#include "Opt.h"
#include "DHCPConst.h"

class TOptStatusCode : public TOpt
{
  public:
    TOptStatusCode( char * &buf,  int &len, TMsg* parent);
    TOptStatusCode(int status,string Message, TMsg* parent);
    
    int getSize();
    char * storeSelf( char* buf);
    
    int getCode();  
    string getText();
    
    bool doDuties();    
 private:
    string Message;
    int StatusCode;
    bool Valid;
};

#endif 
