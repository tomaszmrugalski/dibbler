
#include <gtest/gtest.h>
#include "poslib.h"


using namespace std;

class TSIGTest : public ::testing::Test {
public:
    TSIGTest() : Hostname_("foo"), Domain_("example.org"),
                 TTL_("2h"/* DNSUPDATE_DEFAULT_TTL */), Msg_(NULL), Answer_(NULL) {
        Msg_ = new DnsMessage();
        Msg_->OPCODE = OPCODE_UPDATE;
    }

    void addAAAA(const string& ip, const string& host,
                 const string& zone) {
        domainname domain(zone.c_str());

        ASSERT_TRUE(Msg_);
        DnsRR rr;
        rr.NAME = domainname(Hostname_.c_str(), domain );
        rr.TYPE = qtype_getcode("AAAA", false);
        rr.TTL = txt_to_int(TTL_.c_str());
        string data = rr_fromstring(rr.TYPE, ip.c_str(), domain);
        rr.RDLENGTH = data.size();
        rr.RDATA = (unsigned char*)memdup(data.c_str(), rr.RDLENGTH);

        Msg_->authority.push_back(rr);
    }

    void msgStoreSelf() {
        msgStoreSelf(Msg_);
    }

    void msgStoreSelf(DnsMessage * msg) {
        message_buff buff = msg->compile(UDP_MSG_SIZE);
    }

    void msgSend(const std::string& srvAddr,
                 DnsMessage * msg) {
        _addr server;
        memset(&server, 0, sizeof(server));
        
#ifndef WIN32
        // LINUX, BSD
        server.ss_family = AF_INET6;
#else
        // WINDOWS
        server.sa_family = AF_INET6;
#endif
        txt_to_addr(&server, srvAddr.c_str());
 
        pos_cliresolver res;
	res.udp_tries[0] = 1000; // 1000ms 
	res.n_udp_tries = 1; // just one timeout

        // Answer_ is null here, but response will be set there
	res.query(Msg_, Answer_, &server, Q_NOTCP);
        EXPECT_TRUE(Answer_);
	if (Answer_ && Answer_->RCODE != RCODE_NOERROR) {
	    throw PException((char*)str_rcode(Answer_->RCODE).c_str());
	}
    }
    
    string Hostname_; // = "foo";
    string Domain_; // = "example.org";
    string TTL_; // = string(DNSUPDATE_DEFAULT_TTL);
    DnsMessage * Msg_;
    DnsMessage * Answer_;
    message_buff Buffer_;

};

TEST_F(TSIGTest, AAAA_update) {

    Msg_->questions.push_back(DnsQuestion(domainname(Domain_.c_str()), DNS_TYPE_SOA, CLASS_IN));

    addAAAA("2000::1", Hostname_, Domain_);

    msgSend("2001:db8:1::1", Msg_);


}
