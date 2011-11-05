/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Mateusz Ozga <matozga@gmail.com>
 * changes: Tomek Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 */

#include "IPv6Addr.h"
#include "SmartPtr.h"
#include "Opt.h"

class TOptRtPrefix : public TOpt
{
public:
    TOptRtPrefix(uint32_t lifetime, uint8_t prefixlen, uint8_t metric, SPtr<TIPv6Addr> prefix, TMsg* parent);
    TOptRtPrefix(const char * buf, int bufsize, TMsg* parent);
    char* storeSelf(char* buf);
    int getSize();
    bool doDuties() { return true; };

    uint32_t getLifetime();
    uint8_t getPrefixLen();
    uint8_t getMetric();
    SPtr<TIPv6Addr> getPrefix();
    std::string getPlain();
protected:
    uint32_t Lifetime;
    uint8_t PrefixLen;
    uint8_t Metric;
    SPtr<TIPv6Addr> Prefix;
};

