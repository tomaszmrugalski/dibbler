/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntParsGlobalOpt.h,v 1.9 2007-03-04 22:34:05 thomson Exp $
 *
 */

class TClntParsGlobalOpt;
#ifndef PARSGLOBALOPT_H
#define PARSGLOBALOPT_H

#include "ClntParsIfaceOpt.h"
#include "DHCPConst.h"

class TClntParsGlobalOpt : public TClntParsIfaceOpt
{
public:
    TClntParsGlobalOpt();
    ~TClntParsGlobalOpt();

    void setWorkDir(string dir);
    string getWorkDir();
    void setPrefixLength(int len);
    int getPrefixLength();
    void setDigest(DigestTypes digest);
    DigestTypes getDigest();
    void setScriptsDir(string dir);
    string getScriptsDir();
    void setAnonInfRequest(bool anonymous);
    bool getAnonInfRequest();

private:
    string WorkDir;
    int PrefixLength;
    DigestTypes Digest;
    string ScriptsDir;
    bool   AnonInfRequest;
};

#endif
