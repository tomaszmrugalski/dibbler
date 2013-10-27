/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 */

class TDUID;
#ifndef DUID_H_
#define DUID_H_
#include <vector>
#include <string>
#include <stdint.h>

class TDUID
{
    friend std::ostream& operator<<(std::ostream& out,TDUID &range);
 public:
    TDUID(); // @todo: remove this
    TDUID(const char* DUID,int DUIDlen); // packed
    TDUID(const char* text); // plain
    TDUID(const TDUID &duid);
    TDUID& operator=(const TDUID& duid);
    bool operator==(const TDUID &duid);
    bool operator<=(const TDUID &duid);
    size_t getLen() const;
    char * storeSelf(char* buf);
    const std::string getPlain() const;
    const char * get() const;

    ~TDUID();

private:
    std::vector<uint8_t> DUID_;
    std::string Plain_;
};

#endif
