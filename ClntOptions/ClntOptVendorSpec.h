/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef CLNTOPTVENDORSPEC_H
#define CLNTOPTVENDORSPEC_H

#include "OptVendorSpecInfo.h"

class TClntOptVendorSpec : public TOptVendorSpecInfo
{
public:
    TClntOptVendorSpec(int enterprise, int optionCode, char * data, int dataLen, TMsg* parent);
    TClntOptVendorSpec(char * buf,  int n, TMsg* parent);
    bool doDuties();
 private:
};

#endif /* CLNTOPTVENDORSPECINFO_H */
