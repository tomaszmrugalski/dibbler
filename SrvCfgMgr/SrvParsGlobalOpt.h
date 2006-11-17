/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvParsGlobalOpt.h,v 1.6 2006-11-17 00:44:34 thomson Exp $
 *
 */

#ifndef TSRVPARSGLOBALOPT_H_
#define TSRVPARSGLOBALOPT_H_

#include "SrvParsIfaceOpt.h"
#include "DHCPConst.h"

class TSrvParsGlobalOpt : public TSrvParsIfaceOpt
{
public:
    TSrvParsGlobalOpt(void);
    ~TSrvParsGlobalOpt(void);

    string getWorkDir();
    void setWorkDir(string dir);
    void setStateless(bool stateless);
    bool getStateless();
    void setCacheSize(int bytes);
    int  getCacheSize();
    void addDigest(DigestTypes x);
    List(DigestTypes) getDigest();
    
private:
    string WorkDir;
    bool   Stateless;
    int    CacheSize;
    List(DigestTypes) DigestLst;
};
#endif
