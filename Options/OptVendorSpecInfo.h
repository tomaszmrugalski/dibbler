/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 *
 * released under GNU GPL v2 only licence
 */

#ifndef OPTVENDORSPECINFO_H
#define OPTVENDORSPECINFO_H

#include "Opt.h"
#include "SmartPtr.h"
#include "IPv6Addr.h"
#include <stdint.h>

class TOptVendorSpecInfo : public TOpt
{
  public:
    TOptVendorSpecInfo(uint16_t type, char * buf,  int n, TMsg* parent);
    TOptVendorSpecInfo(uint16_t code, uint32_t enterprise, uint16_t sub_option_code,
                       const char *data, int dataLen, TMsg* parent);
    TOptVendorSpecInfo(uint16_t code, uint32_t enterprise, uint16_t sub_option_code,
                       SPtr<TIPv6Addr> addr, TMsg* parent);
    TOptVendorSpecInfo(uint16_t code, uint32_t enterprise, uint16_t sub_option_code,
                       const std::string& str, TMsg* parent);

    size_t getSize();
    char * storeSelf(char* buf);
    virtual bool isValid() const;
    virtual std::string getPlain();

    uint32_t getVendor();
    ~TOptVendorSpecInfo();
    bool doDuties() { return true; }
protected:

    /// @brief utility function that appends sub-option with specified code and data
    ///
    /// @param sub_option_code the code of suboption to be added
    /// @param data specifies sub-option length
    /// @param data_len pointer to the sub-option data
    void createSuboption(uint16_t sub_option_code, const char* data, size_t data_len);

    uint32_t Vendor_;
};

#endif /* OPTVENDORSPECINFO_H */
