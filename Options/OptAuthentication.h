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

#include <vector>
#include "DHCPConst.h"
#include "Opt.h"
#include "Portable.h"

class TOptAuthentication : public TOpt
{
public:
    const static size_t OPT_AUTH_FIXED_SIZE = 11;

    TOptAuthentication(AuthProtocols proto, uint8_t algo, AuthReplay rdm,
                       TMsg* parent);

    TOptAuthentication(char* buf, size_t buflen, TMsg* parent);

    AuthProtocols getProto() const;
    uint8_t getAlgorithm() const;
    AuthReplay getRDM() const;

    void setRDM(AuthReplay value);

    void setReplayDetection(uint64_t value);
    uint64_t getReplayDetection();

    size_t getSize();
    char * storeSelf(char* buf);
    bool doDuties();

    void setPayload(const std::vector<uint8_t>& data);
    void getPayload(std::vector<uint8_t>& data);

    // not real fields, those are used for checksum calculations
    
    /// @todo: remove this
    // specific for method 4 (see DHCPConst.h)
    uint32_t getSPI() const;
    void setAuthInfoLen(uint16_t len);
    void setDigestType(DigestTypes type);

    inline char* getAuthDataPtr() const { return authDataPtr_; }

    void setRealm(const std::string& realm);

private:
    AuthProtocols proto_; // protocol
    uint8_t algo_;  // algorithm
    AuthReplay rdm_;   // replay detection method
    uint64_t replay_;

    std::vector<uint8_t> data_; ///< auth data (specific to a given proto)

    std::string realm_; ///< auth realm (used in delayed-auth)

    char* authDataPtr_;

    // specific for AUTH_PROTO_DIBBLER
    uint16_t AuthInfoLen_;
};

#endif 
