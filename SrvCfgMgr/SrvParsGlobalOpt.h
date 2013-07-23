/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 */

#ifndef TSRVPARSGLOBALOPT_H_
#define TSRVPARSGLOBALOPT_H_

#include "SrvParsIfaceOpt.h"
#include "DHCPConst.h"

typedef enum {
    SRV_IFACE_ID_ORDER_BEFORE,
    SRV_IFACE_ID_ORDER_AFTER,
    SRV_IFACE_ID_ORDER_NONE
} ESrvIfaceIdOrder;

class TSrvParsGlobalOpt : public TSrvParsIfaceOpt
{
public:
    TSrvParsGlobalOpt(void);
    ~TSrvParsGlobalOpt(void);

    std::string getWorkDir() const;
    void setWorkDir(const std::string& dir);
    void setStateless(bool stateless);
    bool getStateless() const;
    void setCacheSize(int bytes);
    int  getCacheSize() const;
    bool getExperimental() const; // is experimental stuff allowed?
    void setExperimental(bool exper);
    void setInterfaceIDOrder(ESrvIfaceIdOrder order);
    ESrvIfaceIdOrder getInterfaceIDOrder() const;
    void setInactiveMode(bool flex);
    bool getInactiveMode() const;
    void setGuessMode(bool guess);
    bool getGuessMode() const;
    
private:
    bool   Experimental_;
    std::string WorkDir_;
    bool   Stateless_;
    bool   InactiveMode_;
    bool   GuessMode_;
    int    CacheSize_;
    ESrvIfaceIdOrder InterfaceIDOrder_;
};
#endif
