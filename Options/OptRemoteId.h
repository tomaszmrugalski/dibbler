#ifndef OPTREMOTEID_H
#define OPTREMOTEID_H

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Opt.h"

#include <vector>

class TOptRemoteID: public TOpt
{
 public:

    TOptRemoteID(int type, char * buf,  int n, TMsg* parent);
    TOptRemoteID(int type, int enterprise, char *data, int dataLen, TMsg* parent);

    size_t getSize();
    char * storeSelf( char* buf);

    std::vector<uint8_t> getRemoteId();
    std::string getPlain();
    ~TOptRemoteID();
    bool doDuties();
 protected:
    uint32_t EnterpriseId_;
    std::vector<uint8_t> RemoteId_;
    std::string Plain_;
};
#endif // OPTREMOTEID_H
