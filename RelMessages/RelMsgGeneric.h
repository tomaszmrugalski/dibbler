/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef RELMSGGENERIC_H
#define RELMSGGENERIC_H

#include "RelMsg.h"

class TRelMsgGeneric: public TRelMsg {

 public:
    TRelMsgGeneric(int iface, SPtr<TIPv6Addr> addr, char * data, int dataLen);
    string getName();
    bool check();
};

#endif
