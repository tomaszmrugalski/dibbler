#ifndef PARSADDROPT_H_
#define PARSADDROPT_H_

#include "DHCPConst.h"

class TClntParsAddrOpt
{
public:
	TClntParsAddrOpt();
    long getPref();
    void setPref(long pref);
    ESendOpt getPrefSendOpt();
    void setPrefSendOpt(ESendOpt opt);

    long getValid();
    void setValid(long valid);
    ESendOpt getValidSendOpt();
    void setValidSendOpt(ESendOpt opt);
private:
    long Pref;
    ESendOpt PrefSendOpt;
    long Valid;
    ESendOpt ValidSendOpt;

    //	TClntParsAddrOpt(const TClntParsAddrOpt const& ToCopy);
};

#endif
