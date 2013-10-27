/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *
 */

#ifndef CFGMGR_H
#define CFGMGR_H
#include <string>
#include "SmartPtr.h"
#include "DUID.h"
#include "IfaceMgr.h"
#include "Key.h"

/* shared by server and relay */
#define RELAY_MIN_IFINDEX 1024

/* Defined DUID types */

enum EDUIDType{
    DUID_TYPE_NOT_DEFINED = 0,
    DUID_TYPE_LLT = 1,
    DUID_TYPE_EN  = 2,
    DUID_TYPE_LL  = 3
};

class TCfgMgr
{
 public:
    enum DNSUpdateProtocol {
	DNSUPDATE_TCP, /* TCP only */
	DNSUPDATE_UDP, /* UDP only */
	DNSUPDATE_ANY  /* try UDP first, if response truncated, switch to TCP */
    };

    TCfgMgr();
    virtual ~TCfgMgr();

    bool compareConfigs(const std::string& cfgFile, const std::string& oldCfgFile);
    void copyFile(const std::string& cfgFile, const std::string& oldCfgFile);
    SPtr<TDUID> getDUID();
    void setWorkdir(std::string workdir);
    int getLogLevel();
    std::string getWorkDir();
    std::string getLogName();
    void setDDNSProtocol(DNSUpdateProtocol proto);
    DNSUpdateProtocol getDDNSProtocol() { return DdnsProto; }
    void setDDNSTimeout(unsigned int timeout) { DDNSTimeout_ = timeout; }
    unsigned int getDDNSTimeout() { return DDNSTimeout_; }

#if !defined(MOD_SRV_DISABLE_DNSUPDATE) && !defined(MOD_CLNT_DISABLE_DNSUPDATE)
    void addKey(SPtr<TSIGKey> key);
    SPtr<TSIGKey> getKey();
#endif

#ifndef MOD_DISABLE_AUTH
    void setAuthProtocol(AuthProtocols proto);
    void setAuthReplay(AuthReplay replay_detection_mode);
    void setAuthAlgorithm(uint8_t algorithm); // protocol specific value
    AuthProtocols getAuthProtocol();
    AuthReplay getAuthReplay();
    uint8_t getAuthAlgorithm();
    void setAuthDropUnauthenticated(bool drop);
    bool getAuthDropUnauthenticated();
    void setAuthRealm(const std::string& realm);
    std::string getAuthRealm();
#endif

 protected:
    SPtr<TDUID> DUID;
    bool setDUID(const std::string& duidFile, TIfaceMgr &ifaceMgr);
    bool loadDUID(const std::string& filename);
    bool generateDUID(const std::string& duidFile,char * mac,int macLen, int macType);
    std::string Workdir;
    std::string LogName;
    int LogLevel;
    bool IsDone;
    EDUIDType DUIDType;
    int DUIDEnterpriseNumber;
    SPtr<TDUID> DUIDEnterpriseID;
    DNSUpdateProtocol DdnsProto;
    unsigned int DDNSTimeout_;

#ifndef MOD_DISABLE_AUTH
    AuthProtocols AuthProtocol_;
    uint8_t AuthAlgorithm_;
    AuthReplay AuthReplay_;
    std::string AuthRealm_;

    bool AuthDropUnauthenticated_;
#endif

    // for TSIG in DDNS
    TSIGKeyList Keys_;
 private:
    
};
#endif
