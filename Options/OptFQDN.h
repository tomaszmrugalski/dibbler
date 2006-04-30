/*
* Dibbler - a portable DHCPv6
*
* authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
*          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
*          from ENSEEIHT, Toulouse, France
*
* released under GNU GPL v2 licence
*
* $Id: OptFQDN.h,v 1.2 2006-04-30 17:32:35 thomson Exp $
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
  	TOptFQDN(string fqdn, TMsg* parent);
  	
  	/**
  	 * Constructor
  	 * 
  	 * Build the option with a buffer received
  	 * @param buf the buffer received, containing the whole option
  	 * @param bufsize the size of the buffer
  	 * @param parent the message in which this option is included
  	 */
  	TOptFQDN(char * &buf, int &bufsize, TMsg* parent);
  	
  	/**
  	 * Destructor - Does actually nothing
  	 */
  	~TOptFQDN();
  	
    int getSize();
    char * storeSelf( char* buf);
    bool isValid();
    
    bool getNFlag();
    bool getSFlag();
    bool getOFlag();
    void setNFlag(bool flag);
    void setOFlag(bool flag);
    void setSFlag(bool flag);
    string getFQDN();

  	
  private:
  	string fqdn;
  	char *encodedFQDN;
  	bool flag_N;
  	bool flag_O;
  	bool flag_S;
  	bool Valid;
    
};

#endif /* OPTFQDN_H */
