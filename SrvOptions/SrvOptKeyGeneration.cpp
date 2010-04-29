/*
 * Dibbler - a portable DHCPv6
 *
 * author:  Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptKeyGeneration.cpp,v 1.3 2008-08-29 00:07:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2008-06-18 21:56:39  thomson
 * Auth options cleanup.
 *
 * Revision 1.1  2008-02-25 20:42:46  thomson
 * Missing new AUTH files added.
 *
 *
 */

#include "SrvOptKeyGeneration.h"
#include "OptAAAAuthentication.h"
#include "Msg.h"
#include "Logger.h"
#include "SrvCfgMgr.h"

TSrvOptKeyGeneration::TSrvOptKeyGeneration(char * buf,  int n, TMsg* parent)
	:TOptKeyGeneration(buf, n, parent) {
}

#define WATCHDOG_RANDOM 10000

TSrvOptKeyGeneration::TSrvOptKeyGeneration(TSrvMsg* parent)
	:TOptKeyGeneration(parent) {

    uint32_t spi;

    int watchdog = WATCHDOG_RANDOM;
    {
	do {
	    spi = (uint32_t)rand();
	    if (!--watchdog) {
	    Log(Error) << "Auth: Unable to generate unique SPI after " << WATCHDOG_RANDOM << " tries." << LogEnd;
	    return;
	    }
	    
	} while (Parent->AuthKeys->Get(spi));
	
        PrintHex("Auth:generated SPI: ", (char*)&spi, 4);
        Parent->setSPI(spi);
    }

    setLifetime(SrvCfgMgr().getAuthLifetime());
    
    setAlgorithmId(parent->DigestType);


    unsigned kgnlen = SrvCfgMgr().getAuthKeyGenNonceLen();
    char *kgn = new char[kgnlen];
    for (unsigned i = 0; i < kgnlen; i++)
        kgn[i] = (char)(rand() % 256);
    this->Parent->setKeyGenNonce(kgn, kgnlen);
    delete kgn;

    parent->setAuthInfoKey();
}

bool TSrvOptKeyGeneration::doDuties()
{
    return false;
}
