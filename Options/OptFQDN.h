/*
* Dibbler - a portable DHCPv6
*
* authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
*          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
*          from ENSEEIHT, Toulouse, France
*
* released under GNU GPL v2 licence
*
*/
 
/**
 * This class is mother of Client and Server version of the FQDN option.
 * 
 * It implements all common methods
 */
class TOptFQDN;
#ifndef OPTFQDN_H
#define OPTFQDN_H

#include <string>

#include "Opt.h"
#include "DHCPConst.h"

class TOptFQDN : public TOpt
{
  public:
  /**
   * Constructor
   * 
   * The FQDN is set and all flags are 0
   * @param fqdn The FQDN about to be sent
   * @param parent The message in which this option is included
   */
    TOptFQDN(const std::string& fqdn, TMsg* parent);
  	
    /**
     * Constructor
     * 
     * Build the option with a buffer received
     * @param buf the buffer received, containing the whole option
     * @param bufsize the size of the buffer
     * @param parent the message in which this option is included
     */
    TOptFQDN(const char* buf, int bufsize, TMsg* parent);
    
    /**
     * Destructor - Does actually nothing
     */
    ~TOptFQDN();
    
    size_t getSize();
    char * storeSelf( char* buf);
    bool isValid() const;
    
    bool getNFlag() const;
    bool getSFlag() const;
    bool getOFlag() const;
    void setNFlag(bool flag);
    void setOFlag(bool flag);
    void setSFlag(bool flag);
    std::string getFQDN() const;
    virtual std::string getPlain();
    bool doDuties();
  	
  private:
    std::string fqdn_;
    bool flag_N_;
    bool flag_O_;
    bool flag_S_;
};

#endif /* OPTFQDN_H */
