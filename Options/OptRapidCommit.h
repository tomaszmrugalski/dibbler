/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef OPTRAPIDCOMMIT_H
#define OPTRAPIDCOMMIT_H

#include "DHCPConst.h"
#include "Opt.h"

class TOptRapidCommit : public TOpt
{
public:
    TOptRapidCommit(TMsg* parent);
    TOptRapidCommit(char * &buf,  int &n,TMsg* parent);
    int getSize();
    char * storeSelf(char* buf);
};

#endif 
