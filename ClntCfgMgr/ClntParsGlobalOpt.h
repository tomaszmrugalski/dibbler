/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
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

    void setWorkDir(const std::string& dir);
    std::string getWorkDir();
    void setOnLinkPrefixLength(int len);
    int getOnLinkPrefixLength();
    void setAnonInfRequest(bool anonymous);
    bool getAnonInfRequest();
    void setInsistMode(bool insist);
    bool getInsistMode();
    void setInactiveMode(bool flex);
    bool getInactiveMode();

    void setExperimental();
    bool getExperimental();

    void setFQDNFlagS(bool s);
    bool getFQDNFlagS();

    void setConfirm(bool conf);
    bool getConfirm();


private:
    std::string WorkDir;
    int PrefixLength;
    bool   AnonInfRequest;
    bool   InactiveMode;
    bool   InsistMode;
    bool UseConfirm;

    bool FQDNFlagS;

    bool Experimental;
};

#endif
