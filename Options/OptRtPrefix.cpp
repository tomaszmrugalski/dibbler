/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Mateusz Ozga <matozga@gmail.com>
 * changes: Tomek Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 */

#include <sstream>
#include "DHCPConst.h"
#include "OptRtPrefix.h"
#include "Logger.h"
#include "Portable.h"


TOptRtPrefix::TOptRtPrefix(uint32_t lifetime, uint8_t prefixlen, uint8_t metric, SPtr<TIPv6Addr> prefix, TMsg* parent)
    :TOpt(OPTION_RTPREFIX, parent), Lifetime(lifetime), PrefixLen(prefixlen), Metric(metric), Prefix(prefix) {
}

TOptRtPrefix::TOptRtPrefix(const char * buf, int bufsize, TMsg* parent)
    :TOpt(OPTION_RTPREFIX, parent) {
    if (bufsize<20) {
        Log(Warning) << "Received truncated RTPREFIX option" << LogEnd;
        Valid = false;
        return;
    }
    Lifetime = readUint32(buf);
    buf += sizeof(uint32_t);
    bufsize -= sizeof(uint32_t);

    PrefixLen = buf[0];
    buf += 1;
    bufsize -= 1;

    Metric = buf[0];
    buf += 1;
    bufsize -= 1;

    Prefix = new TIPv6Addr(buf, false);
    buf += 16;
    bufsize -= 16;

    Valid = parseOptions(SubOptions, buf, bufsize, parent, OPTION_RTPREFIX);
}

char* TOptRtPrefix::storeSelf(char* buf) {
    buf = storeHeader(buf);
    buf = writeUint32(buf, Lifetime);
    buf[0] = PrefixLen;
    buf[1] = Metric;
    buf += 2;

    buf = Prefix->storeSelf(buf);

    return storeSubOpt(buf);
}

uint32_t TOptRtPrefix::getLifetime()
{
    return Lifetime;
}

uint8_t TOptRtPrefix::getPrefixLen()
{
    return PrefixLen;
}

uint8_t TOptRtPrefix::getMetric()
{
    return Metric;
}

SPtr<TIPv6Addr> TOptRtPrefix::getPrefix()
{
    return Prefix;
}

int TOptRtPrefix::getSize()
{
    return 4+22+getSubOptSize();
}

std::string TOptRtPrefix::getPlain() {
    stringstream tmp;
    tmp << Prefix->getPlain() << "/" << (unsigned int)PrefixLen << " " << Lifetime
        << " " << (unsigned int)Metric;
    return tmp.str();
}
