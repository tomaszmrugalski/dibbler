/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelParsGlobalOpt.h,v 1.6 2008-08-29 00:07:32 thomson Exp $
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
    
    string getWorkDir();
    void setWorkDir(string dir);

    void setGuessMode(bool guess);
    bool getGuessMode();

    void setInterfaceIDOrder(ERelIfaceIdOrder order);
    ERelIfaceIdOrder getInterfaceIDOrder();

    void setRemoteID(SPtr<TOptVendorData> remoteID);
    SPtr<TOptVendorData> getRemoteID();

    void setEcho(SPtr<TRelOptEcho> echo);
    SPtr<TRelOptEcho> getEcho();

private:
    string WorkDir;
    bool GuessMode;

    ERelIfaceIdOrder InterfaceIDOrder;
    SPtr<TOptVendorData> RemoteID;
    SPtr<TRelOptEcho>     Echo;
};
#endif
