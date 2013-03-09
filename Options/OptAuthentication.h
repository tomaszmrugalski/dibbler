/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomek Mrugalski <thomson(at)klub(dot)com(dot)pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef OPTAUTHENTICATION_H
#define OPTAUTHENTICATION_H

#include "DHCPConst.h"
#include "Opt.h"
#include "Portable.h"

class TOptAuthentication : public TOpt
{
public:
    TOptAuthentication(AuthProtocols proto, uint8_t algo, AuthReplay rdm, TMsg* parent);
    TOptAuthentication(char* buf,  int buflen, TMsg* parent);

    AuthProtocols getProto() const;
    uint8_t getAlgorithm() const;
    AuthReplay getRDM() const;

    void setRDM(AuthReplay value);

    void setReplayDetection(uint64_t value);
    uint64_t getReplayDetection();

    size_t getSize();
    char * storeSelf(char* buf);
    bool doDuties();

    // specific for method 4 (see DHCPConst.h)
    uint32_t getSPI() const;
    void setAuthInfoLen(uint16_t len);
    void setDigestType(enum DigestTypes type);

private:
    AuthProtocols proto_; // protocol
    uint8_t algo_;  // algorithm
    AuthReplay rdm_;   // replay detection method
    uint64_t replay_;
    uint16_t AuthInfoLen_;
};

#endif 
