/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntParsGlobalOpt.h,v 1.10 2007-04-01 04:53:18 thomson Exp $
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
    void setInsistMode(bool insist);
    bool getInsistMode();
    void setInactiveMode(bool flex);
    bool getInactiveMode();

private:
    string WorkDir;
    int PrefixLength;
    DigestTypes Digest;
    string ScriptsDir;
    bool   AnonInfRequest;
    bool   InactiveMode;
    bool   InsistMode;
};

#endif
