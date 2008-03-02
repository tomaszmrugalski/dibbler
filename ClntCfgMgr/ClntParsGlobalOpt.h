/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntParsGlobalOpt.h,v 1.14 2008-03-02 23:17:58 thomson Exp $
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

    void setAuthAcceptMethods(List(DigestTypes) lst);
    List(DigestTypes) getAuthAcceptMethods();
    void setAuthEnabled(bool enabled);
    bool getAuthEnabled();

    void setExperimental();
    bool getExperimental();

    void setFQDNFlagS(bool s);
    bool getFQDNFlagS();

private:
    string WorkDir;
    int PrefixLength;
    DigestTypes Digest;
    string ScriptsDir;
    bool   AnonInfRequest;
    bool   InactiveMode;
    bool   InsistMode;
    bool   AuthEnabled;
    List(DigestTypes) AuthAcceptMethods;

    bool FQDNFlagS;

    bool Experimental;
};

#endif
