/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef OPTUSERCLASS_H
#define OPTUSERCLASS_H

#include "Opt.h"
#include <stdint.h>
#include <vector>

class TOptUserClass : public TOpt
{
 public:
    struct UserClassData {
	std::vector<char> opaqueData_;
    };

    std::vector<UserClassData> userClassData_;

    bool parseUserData(const char* buf, unsigned short buf_len);

    TOptUserClass(uint16_t type, const char* buf, unsigned short buf_len, TMsg* parent);
    TOptUserClass(uint16_t type, TMsg* parent);
    size_t getSize();
    virtual bool isValid() const;
    char* storeSelf( char* buf);
    bool doDuties() { return false; }
protected:
    char* storeUserData(char* buf);
};

#endif /* USERCLASS_H */
