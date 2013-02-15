/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef TRELPARSGLOBALOPT_H_
#define TRELPARSGLOBALOPT_H_
#include "RelParsIfaceOpt.h"
#include "OptVendorData.h"
#include "RelOptEcho.h"
#include "SmartPtr.h"
#include <string>

typedef enum {
    REL_IFACE_ID_ORDER_BEFORE,
    REL_IFACE_ID_ORDER_AFTER,
    REL_IFACE_ID_ORDER_NONE
} ERelIfaceIdOrder;

class TRelParsGlobalOpt : public TRelParsIfaceOpt
{
 public:
    TRelParsGlobalOpt(void);
    ~TRelParsGlobalOpt(void);
    
    std::string getWorkDir();
    void setWorkDir(std::string dir);

    void setGuessMode(bool guess);
    bool getGuessMode();

    void setInterfaceIDOrder(ERelIfaceIdOrder order);
    ERelIfaceIdOrder getInterfaceIDOrder();

    void setRemoteID(SPtr<TOptVendorData> remoteID);
    SPtr<TOptVendorData> getRemoteID();

    void setEcho(SPtr<TRelOptEcho> echo);
    SPtr<TRelOptEcho> getEcho();

private:
    std::string WorkDir_;
    bool GuessMode_;

    ERelIfaceIdOrder InterfaceIDOrder_;
    SPtr<TOptVendorData> RemoteID_;
    SPtr<TRelOptEcho> Echo_;
};
#endif
