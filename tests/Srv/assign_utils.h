#include <gtest/gtest.h>
#include "SrvMsgSolicit.h"
#include "SrvMsgAdvertise.h"
#include "SrvMsgRequest.h"
#include "SrvMsgReply.h"
#include "SrvMsgRenew.h"
#include "SrvMsgRebind.h"
#include "SrvMsgRelease.h"
#include "SrvMsgConfirm.h"
#include "SrvMsgDecline.h"
#include "SrvMsgInfRequest.h"
#include "SrvOptIA_NA.h"
#include "SrvOptIA_PD.h"
#include <string>
#include <unistd.h>

namespace test {

    struct Pkt6Info {
        Pkt6Info(int iface, char* msg, int size, SPtr<TIPv6Addr> addr, int port);
        int Iface_;
        std::vector<uint8_t> Data_;
        SPtr<TIPv6Addr> Addr_;
        int Port_;
    };

    typedef std::vector<Pkt6Info> Pkt6Collection;

    class NakedSrvIfaceMgr: public TSrvIfaceMgr {
    public:

        Pkt6Collection sent_pkts_;

        NakedSrvIfaceMgr(const std::string& xmlFile)
            : TSrvIfaceMgr(xmlFile) {
            TSrvIfaceMgr::Instance = this;
        }
        ~NakedSrvIfaceMgr() {
            TSrvIfaceMgr::Instance = NULL;
        }
        virtual bool send(int iface, char *msg, int size, SPtr<TIPv6Addr> addr, int port);
        virtual int receive(unsigned long timeout, char* buf, int& bufsize, SPtr<TIPv6Addr> peer);

    };

    class NakedSrvAddrMgr: public TSrvAddrMgr {
    public:
        NakedSrvAddrMgr(const std::string& config, bool load_db)
            :TSrvAddrMgr(config, load_db) {
            TSrvAddrMgr::Instance = this;
        }
        ~NakedSrvAddrMgr() {
            TSrvAddrMgr::Instance = NULL;
        }
    };

    class NakedSrvCfgMgr : public TSrvCfgMgr {
    public:
        NakedSrvCfgMgr(const std::string& config, const std::string& dbfile)
            :TSrvCfgMgr(config, dbfile) {
            TSrvCfgMgr::Instance = this;
        }
        ~NakedSrvCfgMgr() {
            TSrvCfgMgr::Instance = NULL;
        }
    };

    class NakedSrvTransMgr: public TSrvTransMgr {
    public:
        NakedSrvTransMgr(const std::string& xmlFile, int port)
            :TSrvTransMgr(xmlFile, port) {
            TSrvTransMgr::Instance = this;
        }
        List(TSrvMsg)& getMsgLst() { return MsgLst; }
        ~NakedSrvTransMgr() {
            TSrvTransMgr::Instance =  NULL;
        }
    };

    class ServerTest : public ::testing::Test {
    public:
        ServerTest() {
            clntDuid_ = new TDUID("00:01:00:0a:0b:0c:0d:0e:0f");
            clntId_ = new TOptDUID(OPTION_CLIENTID, clntDuid_, NULL);
            clntAddr_ = new TIPv6Addr("fe80::1234", true);

            ifacemgr_ = new NakedSrvIfaceMgr("testdata/server-IfaceMgr.xml");

            // try to pick up an up and running interface
            ifacemgr_->firstIface();
            while ( (iface_ = ifacemgr_->getIface()) && (!iface_->flagUp() || !iface_->flagRunning())) {
            }

        }

        bool createMgrs(std::string config);

        void createIAs(TMsg * msg) {
            ia_iaid_ = 123;
            ia_ = new TSrvOptIA_NA(ia_iaid_, 100, 200, msg);
            ta_iaid_ = 456;
            ta_ = new TOptTA(ta_iaid_, msg);
            pd_iaid_ = 789;
            pd_ = new TSrvOptIA_PD(pd_iaid_, 100, 200, msg);
        }

        void setIface(const std::string& name);

        SPtr<TSrvMsgSolicit> createSolicit() {

            char empty[] = { SOLICIT_MSG, 0x1, 0x2, 0x3};
            SPtr<TSrvMsgSolicit> sol = new TSrvMsgSolicit(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*sol));
            return sol;
        }

        SPtr<TSrvMsgRequest> createRequest() {
            char empty[] = { REQUEST_MSG, 0x1, 0x2, 0x4};
            SPtr<TSrvMsgRequest> request = new TSrvMsgRequest(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*request));
            return request;
        }

        SPtr<TSrvMsgRenew> createRenew() {
            char empty[] = { RENEW_MSG, 0x1, 0x2, 0x5};
            SPtr<TSrvMsgRenew> renew = new TSrvMsgRenew(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*renew));
            return renew;
        }

        SPtr<TSrvMsgRebind> createRebind() {
            char empty[] = { REBIND_MSG, 0x1, 0x2, 0x6};
            SPtr<TSrvMsgRebind> rebind = new TSrvMsgRebind(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*rebind));
            return rebind;
        }

        SPtr<TSrvMsgRelease> createRelease() {
            char empty[] = { RELEASE_MSG, 0x1, 0x2, 0x7};
            SPtr<TSrvMsgRelease> release = new TSrvMsgRelease(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*release));
            return release;
        }

        SPtr<TSrvMsgDecline> createDecline() {
            char empty[] = { DECLINE_MSG, 0x1, 0x2, 0x8};
            SPtr<TSrvMsgDecline> decline = new TSrvMsgDecline(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*decline));
            return decline;
        }

        SPtr<TSrvMsgConfirm> createConfirm() {
            char empty[] = { CONFIRM_MSG, 0x1, 0x2, 0x9};
            SPtr<TSrvMsgConfirm> confirm = new TSrvMsgConfirm(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*confirm));
            return confirm;
        }

        SPtr<TSrvMsgInfRequest> createInfRequest() {
            char empty[] = { INFORMATION_REQUEST_MSG, 0x1, 0x2, 0xa};
            SPtr<TSrvMsgInfRequest> infrequest = new TSrvMsgInfRequest(iface_->getID(), clntAddr_, empty, sizeof(empty));
            createIAs(&(*infrequest));
            return infrequest;
        }

        SPtr<TSrvMsg> sendAndReceive(SPtr<TSrvMsg> clntMsg, unsigned int expectedMsgCount = 1);

        bool checkIA_NA(SPtr<TSrvOptIA_NA> ia, SPtr<TIPv6Addr> minRange,
                        SPtr<TIPv6Addr> maxRange, uint32_t iaid, uint32_t t1,
                        uint32_t t2, uint32_t pref, uint32_t valid);

        bool checkIA_PD(SPtr<TSrvOptIA_PD> ia, SPtr<TIPv6Addr> minRange,
                        SPtr<TIPv6Addr> maxRange, uint32_t iaid, uint32_t t1,
                        uint32_t t2, uint32_t pref, uint32_t valid, uint8_t prefixLen);

        void addRelayInfo(const std::string& linkAddr, const std::string& peerAddr,
                          uint8_t hopCount, const TOptList& echoList);

        void sendHex(const std::string& src_addr, uint16_t src_port,
                     const std::string& dst_addr, uint16_t dst_port,
                     const std::string& iface_name,
                     const std::string& hex_data);

        void clearRelayInfo();
        void setRelayInfo(SPtr<TSrvMsg> msg);

        ~ServerTest();

        NakedSrvIfaceMgr * ifacemgr_;
        NakedSrvCfgMgr * cfgmgr_;
        NakedSrvAddrMgr * addrmgr_;
        NakedSrvTransMgr * transmgr_;
        SPtr<TIfaceIface> iface_;
        SPtr<TSrvCfgIface> cfgIface_;

        SPtr<TIPv6Addr> clntAddr_;
        SPtr<TDUID> clntDuid_;
        SPtr<TOptDUID> clntId_;

        SPtr<TSrvOptIA_NA> ia_;
        SPtr<TSrvOptIA_PD> pd_;
        SPtr<TOptTA>    ta_;
        uint32_t ia_iaid_;
        uint32_t ta_iaid_;
        uint32_t pd_iaid_;

        // Relay info
        std::vector<TSrvMsg::RelayInfo> relayInfo_;
    };

} // namespace test
