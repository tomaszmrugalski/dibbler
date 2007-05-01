/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelParsGlobalOpt.h,v 1.4 2007-05-01 12:03:13 thomson Exp $
 *
 */

#ifndef TRELPARSGLOBALOPT_H_
#define TRELPARSGLOBALOPT_H_
#include "RelParsIfaceOpt.h"
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
    
private:
    string WorkDir;
    bool GuessMode;

    ERelIfaceIdOrder InterfaceIDOrder;
};
#endif
