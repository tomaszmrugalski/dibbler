/*
 * Dibbler - a portable DHCPv6
 *
 * author:  Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef MOD_DISABLE_AUTH

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
	
        PrintHex("Auth:generated SPI: ", (uint8_t*)&spi, 4);
        Parent->setSPI(spi);
    }

    setLifetime(SrvCfgMgr().getAuthLifetime());
    
    setAlgorithmId(parent->digestType_);


    unsigned kgnlen = SrvCfgMgr().getAuthKeyGenNonceLen();
    char *kgn = new char[kgnlen];
    for (unsigned i = 0; i < kgnlen; i++)
        kgn[i] = (char)(rand() % 256);
    this->Parent->setKeyGenNonce(kgn, kgnlen);
    delete [] kgn;

    parent->setAuthInfoKey();
}

bool TSrvOptKeyGeneration::doDuties()
{
    return false;
}

#endif
