/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntParsGlobalOpt.h,v 1.19 2008-08-30 21:41:11 thomson Exp $
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
    void setNotifyScripts(bool useScripts);
    bool getNotifyScripts();
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

    void setMappingPrefix(bool useMP);
    bool getMappingPrefix();

    void setFQDNFlagS(bool s);
    bool getFQDNFlagS();

    void setConfirm(bool conf);
    bool getConfirm();


private:
    string WorkDir;
    int PrefixLength;
    DigestTypes Digest;
    string ScriptsDir;
    bool   NotifyScripts;
    bool   AnonInfRequest;
    bool   InactiveMode;
    bool   InsistMode;
    bool   AuthEnabled;
    List(DigestTypes) AuthAcceptMethods;
    bool UseConfirm;

    bool FQDNFlagS;

    bool Experimental;
    bool ExperimentalMappingPrefix;
};

#endif
