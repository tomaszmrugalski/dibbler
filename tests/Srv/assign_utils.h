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

    /// @brief holds information about received IPv6 packet data
    struct Pkt6Info {
        Pkt6Info(int iface, char* msg, int size, SPtr<TIPv6Addr> addr, int port);
        int Iface_;
        std::vector<uint8_t> Data_;
        SPtr<TIPv6Addr> Addr_;
        int Port_;
    };

    /// @brief a collection of received (unparsed) packets
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
        virtual int receive(unsigned long timeout, char* buf, int& bufsize,
                            SPtr<TIPv6Addr> peer, SPtr<TIPv6Addr> myaddr);

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

        virtual void sendPacket(SPtr<TSrvMsg> msg);

        SrvMsgList& getMsgLst() { return MsgLst_; }

        ~NakedSrvTransMgr() {
            TSrvTransMgr::Instance =  NULL;
        }

        SrvMsgList MsgLst_;
    };

    class ServerTest : public ::testing::Test {
    public:
        ServerTest();

        bool createMgrs(std::string config);

        void createIAs(TMsg * msg);

        void setIface(const std::string& name);

        /// @brief creates SOLICIT message and IA_NA, IA_TA, IA_PD options
        SPtr<TSrvMsgSolicit> createSolicit();

        /// @brief creates REQUEST message and IA_NA, IA_TA, IA_PD options
        SPtr<TSrvMsgRequest> createRequest();

        /// @brief creates RENEW message and IA_NA, IA_TA, IA_PD options
        SPtr<TSrvMsgRenew> createRenew();

        /// @brief creates REBIND message and IA_NA, IA_TA, IA_PD options
        SPtr<TSrvMsgRebind> createRebind();

        /// @brief creates RELEASE message and IA_NA, IA_TA, IA_PD options
        SPtr<TSrvMsgRelease> createRelease();

        /// @brief creates DECLINE message and IA_NA, IA_TA, IA_PD options
        SPtr<TSrvMsgDecline> createDecline();

        /// @brief creates CONFIRM message and IA_NA, IA_TA, IA_PD options
        SPtr<TSrvMsgConfirm> createConfirm();

        /// @brief creates INF-REQUEST message and IA_NA, IA_TA, IA_PD options
        SPtr<TSrvMsgInfRequest> createInfRequest();

        /// @brief creates an IAPREFIX option with specified parameters
        ///
        /// @param addr_txt text representation of the prefix
        /// @param len prefix length
        /// @param pref preferred lifetime (in seconds)
        /// @param valid valid lifetime (in seconds)
        ///
        /// @return IAPREFIX option
        TOptPtr createPrefix(const std::string& addr_txt, uint8_t len, uint32_t pref, uint32_t valid);


        /// @brief Conducts prefix delegation assignment test
        ///
        /// Sends SOLCIT with IA_PD, checks that the ADVERTISE contains proper response
        /// with expected values, then sends REQUEST and repeats the checks for
        /// received REPLY.
        ///
        /// @param config
        /// @param pd_to_be_sent
        /// @param min_range
        /// @param max_range
        /// @param expected_prefix_len
        /// @param expected_iaid
        /// @param expected_t1
        /// @param expected_t2
        /// @param expected_pref
        /// @param expected_valid
        void prefixText(const std::string& config, const TOptPtr& pd_to_be_sent,
                        const std::string& min_range, const std::string& max_range,
                        uint8_t expected_prefix_len, uint32_t expected_iaid,
                        uint32_t expected_t1, uint32_t expected_t2,
                        uint32_t expected_pref, uint32_t expected_valid);


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
