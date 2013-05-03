#ifndef OPTREMOTEID_H
#define OPTREMOTEID_H

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Opt.h"

class TOptRemoteID: public TOpt
{
public:

  TOptRemoteID(int type, char * buf,  int n, TMsg* parent);
  TOptRemoteID(int type, int enterprise, char *data, int dataLen, TMsg* parent);

  size_t getSize();
  char * storeSelf( char* buf);
  bool isValid();

  unsigned int getRemoteId();
  std::string getPlain();
  ~TOptRemoteID();
  bool doDuties();
protected:
  unsigned int RemoteId;
  unsigned int EnterpriseNumber;
  std::string Plain;

};
#endif // OPTREMOTEID_H
