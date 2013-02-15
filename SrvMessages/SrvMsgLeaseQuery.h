/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TSrvMsgLeaseQuery;
#ifndef SRVMSGLEASEQUERY_H
#define SRVMSGLEASEQUERY_H

#include "SmartPtr.h"
#include "SrvMsg.h"
#include "IPv6Addr.h"

class TSrvMsgLeaseQuery : public TSrvMsg
{
  public:
    TSrvMsgLeaseQuery(int iface, SPtr<TIPv6Addr> addr, char* buf,
                      int bufSize, bool tcp = false);

    void doDuties();
    bool check();
    unsigned int getBulkSize(char * buf);
    ~TSrvMsgLeaseQuery();
    std::string getName() const;
    bool isTCP();
  private:
    bool tcp;
};


#endif /* SRVMSGREQUEST_H */
