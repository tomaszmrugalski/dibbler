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

namespace test {
    class NakedSrvIfaceMgr: public TSrvIfaceMgr {
    public:
        NakedSrvIfaceMgr(const std::string& xmlFile)
            : TSrvIfaceMgr(xmlFile) {
            TSrvIfaceMgr::Instance = this;
        }
        ~NakedSrvIfaceMgr() {
            TSrvIfaceMgr::Instance = NULL;
        }
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
        NakedSrvTransMgr(const std::string& xmlFile): TSrvTransMgr(xmlFile) {
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

        SPtr<TSrvMsg> sendAndReceive(SPtr<TSrvMsg> clntMsg, int expectedMsgCount = 1);

        bool checkIA_NA(SPtr<TSrvOptIA_NA> ia, SPtr<TIPv6Addr> minRange,
                        SPtr<TIPv6Addr> maxRange, uint32_t iaid, uint32_t t1,
                        uint32_t t2, uint32_t pref, uint32_t valid);

        bool checkIA_PD(SPtr<TSrvOptIA_PD> ia, SPtr<TIPv6Addr> minRange,
                        SPtr<TIPv6Addr> maxRange, uint32_t iaid, uint32_t t1,
                        uint32_t t2, uint32_t pref, uint32_t valid, uint8_t prefixLen);

        ~ServerTest() {
            delete transmgr_;
            delete cfgmgr_;
            delete addrmgr_;
            delete ifacemgr_;
        }

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
    };

} // namespace test
