/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <unistd.h>
#include <iostream>
#include "ClntIfaceIface.h"
#include "Portable.h"
#include "Logger.h"
#include "DHCPDefaults.h"
#ifdef MINGWBUILD
#include <io.h>
#endif

using namespace std;

/*
 * stores informations about interface
 */
TClntIfaceIface::TClntIfaceIface(char * name, int id, unsigned int flags, char* mac,
                                 int maclen, char* llAddr, int llAddrCnt, char * globalAddr,
                                 int globalAddrCnt, int hwType)
  :TIfaceIface(name, id, flags, mac, maclen, llAddr, llAddrCnt, globalAddr, globalAddrCnt, hwType),
   TunnelEndpoint(0), LifetimeTimeout(DHCPV6_INFINITY) {

    LifetimeTimestamp = (uint32_t)time(NULL);

    this->DNSServerLst.clear();
    this->DomainLst.clear();
    this->NTPServerLst.clear();
    this->Timezone = "";

    DnsConfigured = ! FLUSH_OTHER_CONFIGURED_DNS_SERVERS;

    unlink(OPTION_DNS_SERVERS_FILENAME);
    unlink(OPTION_DOMAINS_FILENAME);
    unlink(OPTION_NTP_SERVERS_FILENAME);
    unlink(OPTION_TIMEZONE_FILENAME);
    unlink(OPTION_SIP_SERVERS_FILENAME);
    unlink(OPTION_SIP_DOMAINS_FILENAME);
    unlink(OPTION_NIS_SERVERS_FILENAME);
    unlink(OPTION_NIS_DOMAIN_FILENAME);
    unlink(OPTION_NISP_SERVERS_FILENAME);
    unlink(OPTION_NISP_DOMAIN_FILENAME);

    setPrefixLength(CLIENT_DEFAULT_PREFIX_LENGTH);
}

/*
 * this method returns timeout to nearest option renewal
 */
unsigned int TClntIfaceIface::getTimeout() {
    if (this->LifetimeTimeout == DHCPV6_INFINITY)
        return DHCPV6_INFINITY;
    unsigned int current = (uint32_t)time(NULL);
    if (current > this->LifetimeTimestamp+this->LifetimeTimeout)
        return 0;
    return this->LifetimeTimestamp+this->LifetimeTimeout-current;
}

bool TClntIfaceIface::setDNSServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv,
                                      List(TIPv6Addr) addrs) {

#ifdef WIN32
    //remove all dns records before the first configuration
    if (!DnsConfigured) {
        Log(Notice) << "Removing all DNS servers on interface "
                    << getFullName() << " (not configured yet)." << LogEnd;
        dns_del(this->getName(), this->getID(), NULL);
        DnsConfigured = true;
    }
#endif

    // remove old addresses
    SPtr<TIPv6Addr> old, addr;
    this->DNSServerLst.first();
    while (old = this->DNSServerLst.get()) {
        // for each already set server...
        addrs.first();
        bool found = false;
        while (addr = addrs.get()) {
            // ... check if it's on the new list
            if ( *addr == *old ) {
                found = true;
                break;
            }
        }
        if (!found) {
            // it's not on the new list, so remove it
            Log(Notice) << "Removing DNS server " << *old << " on interface "
                        << this->getName() << "/" << this->getID() << " (no longer valid)." << LogEnd;
            dns_del(this->getName(), this->getID(), old->getPlain());
            this->delString(OPTION_DNS_SERVERS_FILENAME,old->getPlain());
            this->DNSServerLst.del();
        }
    }

    // add new addresses
    addrs.first();
    while (addr = addrs.get()) {
        this->DNSServerLst.first();
        bool found = false;
        while (old = this->DNSServerLst.get()) {
            if ( *addr == *old ) {
                found = true;
                break;
            }
        }
        if (found) {
            // this address is already set
            Log(Info) << "DNS server " << *addr << " is already set on interface "
                      << this->getName() << "/" << this->getID() << ", no change is needed." << LogEnd;
        } else {
            // it's new address,set it!
            Log(Notice) << "Setting up DNS server " << * addr << " on interface "
                        << this->getName() << "/" << this->getID() << "." << LogEnd;
            dns_add(this->getName(), this->getID(), addr->getPlain());
            this->addString(OPTION_DNS_SERVERS_FILENAME,addr->getPlain());
            this->DNSServerLst.append(addr);
        }
    }
    return true;
}

bool TClntIfaceIface::setDomainLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(std::string) domains) {
    // remove old domains
    SPtr<string> old, domain;
    this->DomainLst.first();
    while (old = this->DomainLst.get()) {
        // for each already domain ...
        domains.first();
        bool found = false;
        while (domain = domains.get()) {
            // ... check if it's on the new list
            if ( *domain == *old ) {
                found = true;
                break;
            }
        }
        if (!found) {
            // it's not on the new list, so remove it
            Log(Notice) << "Removing domain " << *old << " from interface "
                        << this->getName() << "/" << this->getID() << " (no longer valid)." << LogEnd;
            domain_del(this->getName(), this->getID(), old->c_str());
            this->delString(OPTION_DOMAINS_FILENAME,old->c_str());
            this->DomainLst.del();
        }
    }

    // add new domains
    domains.first();
    while (domain = domains.get()) {
        this->DomainLst.first();
        bool found = false;
        while (old = this->DomainLst.get()) {
            if ( *domain == *old ) {
                found = true;
                break;
            }
        }
        if (found) {
            // this domain is already set
            Log(Info) << "Domain " << *domain << " is already set on interface "
                      << this->getName() << "/" << this->getID() << ", no change is needed." << LogEnd;
        } else {
            // it's new domain, set it!
            Log(Notice) << "Setting up Domain " << * domain << " on interface "
                        << this->getName() << "/" << this->getID() << "." << LogEnd;
            domain_add(this->getName(), this->getID(), domain->c_str());
            this->addString(OPTION_DOMAINS_FILENAME,domain->c_str());
            this->DomainLst.append(domain);
        }
    }
    return true;
}

bool TClntIfaceIface::setNTPServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv,
                                      List(TIPv6Addr) addrs) {
    // remove old addresses
    SPtr<TIPv6Addr> old, addr;
    this->NTPServerLst.first();
    while (old = this->NTPServerLst.get()) {
        // for each already set server...
        addrs.first();
        bool found = false;
        while (addr = addrs.get()) {
            // ... check if it's on the new list
            if ( *addr == *old ) {
                found = true;
                break;
            }
        }
        if (!found) {
            // it's not on the new list, so remove it
            Log(Notice) << "Removing NTP server " << *old << " on interface "
                        << this->getName() << "/" << this->getID() << " (no longer valid)." << LogEnd;
            ntp_del(this->getName(), this->getID(), old->getPlain());
            this->delString(OPTION_NTP_SERVERS_FILENAME,old->getPlain());
            this->NTPServerLst.del();
        }
    }

    // add new addresses
    addrs.first();
    while (addr = addrs.get()) {
        this->NTPServerLst.first();
        bool found = false;
        while (old = this->NTPServerLst.get()) {
            if ( *addr == *old ) {
                found = true;
                break;
            }
        }
        if (found) {
            // this address is already set
            Log(Info) << "NTP server " << *addr << " is already set on the interface "
                      << this->getName() << "/" << this->getID() << ", no change is needed." << LogEnd;
        } else {
            // it's new address,set it!
            Log(Notice) << "Setting up NTP server " << * addr << " on the interface "
                        << this->getName() << "/" << this->getID() << "." << LogEnd;
            ntp_add(this->getName(), this->getID(), addr->getPlain());
            this->addString(OPTION_NTP_SERVERS_FILENAME,addr->getPlain());
            this->NTPServerLst.append(addr);
        }
    }
    return true;
}

bool TClntIfaceIface::setTimezone(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, const std::string& timezone) {
    if (timezone==this->Timezone) {
        // timezone has not changed
        Log(Info) << "Timezone " << timezone << " is already set on the interface "
                  << this->getName() << "/" << this->getID() << ", no change is needed." << LogEnd;
        return true;
    }
    this->Timezone = timezone;

    Log(Notice) << "Setting up timezone " << timezone << " (obtained on the interface "
                << this->getName() << "/" << this->getID() << ")." << LogEnd;
    timezone_set(this->getName(), this->getID(), timezone.c_str());
    this->setString(OPTION_TIMEZONE_FILENAME,timezone.c_str());
    return true;
}

bool TClntIfaceIface::setSIPServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv,
                                      List(TIPv6Addr) addrs) {
    // remove old addresses
    SPtr<TIPv6Addr> old, addr;
    this->SIPServerLst.first();
    while (old = this->SIPServerLst.get()) {
        // for each already set server...
        addrs.first();
        bool found = false;
        while (addr = addrs.get()) {
            // ... check if it's on the new list
            if ( *addr == *old ) {
                found = true;
                break;
            }
        }
        if (!found) {
            // it's not on the new list, so remove it
            Log(Notice) << "Removing SIP server " << *old << " on interface "
                        << this->getName() << "/" << this->getID() << " (no longer valid)." << LogEnd;
            sipserver_del(this->getName(), this->getID(), old->getPlain());
            this->delString(OPTION_SIP_SERVERS_FILENAME,old->getPlain());
            this->SIPServerLst.del();
        }
    }

    // add new addresses
    addrs.first();
    while (addr = addrs.get()) {
        this->SIPServerLst.first();
        bool found = false;
        while (old = this->SIPServerLst.get()) {
            if ( *addr == *old ) {
                found = true;
                break;
            }
        }
        if (found) {
            // this address is already set
            Log(Info) << "SIP server " << *addr << " is already set on interface "
                      << this->getName() << "/" << this->getID() << ", no change is needed." << LogEnd;
        } else {
            // it's new address,set it!
            Log(Notice) << "Setting up SIP server " << * addr << " on interface "
                        << this->getName() << "/" << this->getID() << "." << LogEnd;
            sipserver_add(this->getName(), this->getID(), addr->getPlain());
            this->addString(OPTION_SIP_SERVERS_FILENAME,addr->getPlain());
            this->SIPServerLst.append(addr);
        }
    }
    return true;
}
bool TClntIfaceIface::setSIPDomainLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(std::string) domains) {
    // remove old domains
    SPtr<string> old, domain;
    this->SIPDomainLst.first();
    while (old = this->SIPDomainLst.get()) {
        // for each already domain ...
        domains.first();
        bool found = false;
        while (domain = domains.get()) {
            // ... check if it's on the new list
            if ( *domain == *old ) {
                found = true;
                break;
            }
        }
        if (!found) {
            // it's not on the new list, so remove it
            Log(Notice) << "Removing SIP domain " << *old << " from interface "
                        << this->getName() << "/" << this->getID() << " (no longer valid)." << LogEnd;
            sipdomain_del(this->getName(), this->getID(), old->c_str());
            this->delString(OPTION_SIP_DOMAINS_FILENAME,old->c_str());
            this->SIPDomainLst.del();
        }
    }

    // add new domains
    domains.first();
    while (domain = domains.get()) {
        this->SIPDomainLst.first();
        bool found = false;
        while (old = this->SIPDomainLst.get()) {
            if ( *domain == *old ) {
                found = true;
                break;
            }
        }
        if (found) {
            // this domain is already set
            Log(Info) << "SIP domain " << *domain << " is already set on interface "
                      << this->getName() << "/" << this->getID() << ", no change is needed." << LogEnd;
        } else {
            // it's new domain, set it!
            Log(Notice) << "Setting up SIP domain " << * domain << " on interface "
                        << this->getName() << "/" << this->getID() << "." << LogEnd;
            sipdomain_add(this->getName(), this->getID(), domain->c_str());
            this->addString( OPTION_SIP_DOMAINS_FILENAME, domain->c_str() );
            this->SIPDomainLst.append(domain);
        }
    }
    return true;
}

bool TClntIfaceIface::setFQDN(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, const std::string& fqdn) {
    FQDN = fqdn;
    return true;
}

bool TClntIfaceIface::setNISServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs) {
    // remove old addresses
    SPtr<TIPv6Addr> old, addr;
    this->NISServerLst.first();
    while (old = this->NISServerLst.get()) {
        // for each already set server...
        addrs.first();
        bool found = false;
        while (addr = addrs.get()) {
            // ... check if it's on the new list
            if ( *addr == *old ) {
                found = true;
                break;
            }
        }
        if (!found) {
            // it's not on the new list, so remove it
            Log(Notice) << "Removing NIS server " << *old << " on interface "
                        << this->getName() << "/" << this->getID() << " (no longer valid)." << LogEnd;
            nisserver_del(this->getName(), this->getID(), old->getPlain());
            this->delString(OPTION_NIS_SERVERS_FILENAME,old->getPlain());
            this->NISServerLst.del();
        }
    }

    // add new addresses
    addrs.first();
    while (addr = addrs.get()) {
        this->NISServerLst.first();
        bool found = false;
        while (old = this->NISServerLst.get()) {
            if ( *addr == *old ) {
                found = true;
                break;
            }
        }
        if (found) {
            // this address is already set
            Log(Info) << "NIS server " << *addr << " is already set on interface "
                      << this->getName() << "/" << this->getID() << ", no change is needed." << LogEnd;
        } else {
            // it's new address,set it!
            Log(Notice) << "Setting up NIS server " << * addr << " on interface "
                        << this->getName() << "/" << this->getID() << "." << LogEnd;
            nisserver_add(this->getName(), this->getID(), addr->getPlain());
            this->addString(OPTION_NIS_SERVERS_FILENAME, addr->getPlain());
            this->NISServerLst.append(addr);
        }
    }
    return true;
}
bool TClntIfaceIface::setNISDomain(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, const std::string& domain) {
    if (domain==this->NISDomain) {
        // NIS Domain has not changed
        Log(Info) << "NIS Domain " << timezone << " is already set on the interface "
                  << this->getName() << "/" << this->getID() << ", no change is needed." << LogEnd;
        return true;
    }
    this->NISDomain = domain;

    Log(Notice) << "Setting up NIS domain " << domain << " on the interface "
                << this->getName() << "/" << this->getID() << "." << LogEnd;
    nisdomain_set(this->getName(), this->getID(), domain.c_str());
    this->setString( OPTION_NIS_DOMAIN_FILENAME, domain.c_str() );
    return true;
}

bool TClntIfaceIface::setNISPServerLst(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs) {
    // remove old addresses
    SPtr<TIPv6Addr> old, addr;
    this->NISPServerLst.first();
    while (old = this->NISPServerLst.get()) {
        // for each already set server...
        addrs.first();
        bool found = false;
        while (addr = addrs.get()) {
            // ... check if it's on the new list
            if ( *addr == *old ) {
                found = true;
                break;
            }
        }
        if (!found) {
            // it's not on the new list, so remove it
            Log(Notice) << "Removing NIS+ server " << *old << " on interface "
                        << this->getName() << "/" << this->getID() << " (no longer valid)." << LogEnd;
            nisplusserver_del(this->getName(), this->getID(), old->getPlain());
            this->delString(OPTION_NISP_SERVERS_FILENAME,old->getPlain());
            this->NISPServerLst.del();
        }
    }

    // add new addresses
    addrs.first();
    while (addr = addrs.get()) {
        this->NISPServerLst.first();
        bool found = false;
        while (old = this->NISPServerLst.get()) {
            if ( *addr == *old ) {
                found = true;
                break;
            }
        }
        if (found) {
            // this address is already set
            Log(Info) << "NIS+ server " << *addr << " is already set on interface "
                      << this->getName() << "/" << this->getID() << ", no change is needed." << LogEnd;
        } else {
            // it's new address,set it!
            Log(Notice) << "Setting up NIS+ server " << * addr << " on interface "
                        << this->getName() << "/" << this->getID() << "." << LogEnd;
            nisplusserver_add(this->getName(), this->getID(), addr->getPlain());
            this->addString(OPTION_NISP_SERVERS_FILENAME,addr->getPlain());
            this->NISPServerLst.append(addr);
        }
    }
    return true;
}
bool TClntIfaceIface::setNISPDomain(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, const std::string& domain) {
    if (domain==this->NISPDomain) {
        // NIS+ Domain has not changed
        Log(Info) << "NIS+ Domain " << timezone << " is already set on the interface "
                  << this->getName() << "/" << this->getID() << ", no change is needed." << LogEnd;
        return true;
    }
    this->NISPDomain = domain;

    Log(Notice) << "Setting up NIS+ domain " << domain << " on the interface "
                << this->getName() << "/" << this->getID() << "." << LogEnd;
    nisplusdomain_set(this->getName(), this->getID(), domain.c_str());
    this->setString(OPTION_NISP_DOMAIN_FILENAME, domain.c_str());
    return true;
}

bool TClntIfaceIface::setLifetime(SPtr<TDUID> duid, SPtr<TIPv6Addr> srv, unsigned int life) {
    this->LifetimeTimeout = life;
    this->LifetimeTimestamp = (uint32_t)time(NULL);
    if (life == DHCPV6_INFINITY) {
        Log(Info) << "Granted options are parmanent (lifetime = INFINITY.)" << LogEnd;
        return true;
    }
    Log(Info) << "Next option renewal in " << life << " seconds ." << LogEnd;
    return true;
}

TClntIfaceIface::~TClntIfaceIface() {
    this->removeAllOpts();
}

void TClntIfaceIface::addString(const char * filename, const char * str) {
    FILE * f;
    if ( !(f=fopen(filename,"a+"))) {
        Log(Debug) << "Unable to add [" << str << "] to the " << filename << " file." << LogEnd;
        return;
    }
    fprintf(f,"%s\n", str);
    fclose(f);
}

void TClntIfaceIface::delString(const char * filename, const char * str) {
    FILE *f, *fout;
    char buf[512];
    unsigned int len = (unsigned int)strlen(str);
    bool found = false;
    string fileout(filename);

    fileout += "-old";

    if ( !(f=fopen(filename,"r"))) {
        Log(Debug) << "Unable to open file " << filename <<" while trying to delete " << str
                   << " line." << LogEnd;
        return;
    }

    if ( !(fout=fopen(fileout.c_str(),"w"))) {
        Log(Debug) << "Unable to create/overwrite file " << fileout << " while trying to delete "
                   << str << " line." << LogEnd;
        fclose(f);
        return;
    }

    while (!feof(f)) {
        if (!fgets(buf,511,f)) {
            break;
        }
        if ( !found && (strlen(buf)==len) && !strncmp(str,buf,len) ) {
            found = true;
        } else {
            fprintf(fout,"%s",buf);
        }
    }

    fclose(f);    // close original file
    fclose(fout); // file with specified string cut out

    unlink(filename); // delete old version
    rename(fileout.c_str(), filename);
    return;
}

void TClntIfaceIface::setString(const char * filename, const char * str) {
    FILE * f;
    if ( !(f=fopen(filename,"w"))) {
        Log(Debug) << "Unable to write [" << str << "] to the " << filename << " file." << LogEnd;
        return;
    }
    fprintf(f,"%s\n", str);
    fclose(f);
}

void TClntIfaceIface::removeAllOpts() {
    SPtr<TIPv6Addr> addr;
    SPtr<string> str;

    // --- option: DNS-SERVER ---
    this->DNSServerLst.first();
    while (addr = this->DNSServerLst.get()) {
        Log(Notice) << "DNS server " << *addr << " removed from the "
                   << this->getName() << "/" << this->getID() <<" interface." << LogEnd;
        this->DNSServerLst.del();
        dns_del(this->getName(), this->getID(), addr->getPlain());
    }

    // --- option: DOMAIN ---
    this->DomainLst.first();
    while (str = this->DomainLst.get()) {
        Log(Notice) << "Domain " << *str << " removed from the "
                    << this->getName() << "/" << this->getID() <<" interface." << LogEnd;
        this->DomainLst.del();
        domain_del( this->getName(), this->getID(), str->c_str() );
    }

    // --- option: NTP-SERVER ---
    this->NTPServerLst.first();
    while (addr = this->NTPServerLst.get()) {
        Log(Notice) << "NTP server " << *addr << " removed from the "
                    << this->getName() << "/" << this->getID() <<" interface." << LogEnd;
        this->NTPServerLst.del();
        ntp_del(this->getName(), this->getID(), addr->getPlain());
    }

    // --- option: TIMEZONE ---
    if (this->Timezone.length()) {
        Log(Notice) << "Timezone " << this->Timezone << " unset on the interface "
        << this->getName() << "/" << this->getID() <<"." << LogEnd;
        this->Timezone = "";
        timezone_del( this->getName(), this->getID(), this->Timezone.c_str() );
    }

    // --- option: SIP-SERVER ---
    this->SIPServerLst.first();
    while (addr = this->SIPServerLst.get()) {
        Log(Notice) << "SIP server " << *addr << " removed from the "
                    << this->getName() << "/" << this->getID() <<" interface." << LogEnd;
        this->SIPServerLst.del();
        sipserver_del(this->getName(), this->getID(), addr->getPlain());
    }

    // --- option: SIP-DOMAIN ---
    this->SIPDomainLst.first();
    while (str = this->SIPDomainLst.get()) {
        Log(Notice) << "SIP domain " << *str << " removed from the "
                    << this->getName() << "/" << this->getID() << " interface." << LogEnd;
        this->SIPDomainLst.del();
        sipdomain_del( this->getName(), this->getID(), str->c_str() );
    }

    // --- option: NIS-SERVER ---
    this->NISServerLst.first();
    while (addr = this->NISServerLst.get()) {
        Log(Notice) << "NIS server " << *addr << " removed from the "
                    << this->getName() << "/" << this->getID() <<" interface." << LogEnd;
        this->NISServerLst.del();
        nisserver_del(this->getName(), this->getID(), addr->getPlain());
    }

    // --- option: NIS-DOMAIN ---
    if (this->NISDomain.length()) {
        Log(Notice) << "NIS Domain " << this->NISDomain << " unset on the "
        << this->getName() << "/" << this->getID() <<" interface." << LogEnd;

        nisdomain_del( this->getName(), this->getID(), this->NISDomain.c_str() );
    }

    // --- option: NIS+-SERVER ---
    this->NISPServerLst.first();
    while (addr = this->NISPServerLst.get()) {
        Log(Notice) << "NIS+ server " << *addr << " removed from the "
                    << this->getName() << "/" << this->getID() <<" interface." << LogEnd;
        this->NISPServerLst.del();
        nisplusserver_del(this->getName(), this->getID(), addr->getPlain());
    }

    // --- option: NIS+-DOMAIN ---
    if (this->NISPDomain.length()) {
        Log(Notice) << "NIS+ domain " << this->NISDomain << " unset on the "
                    << this->getName() << "/" << this->getID() <<" interface." << LogEnd;
        nisplusdomain_del( this->getName(), this->getID(), this->NISPDomain.c_str() );
    }
}

string TClntIfaceIface::getFQDN() {
        return FQDN;
}

List(TIPv6Addr) TClntIfaceIface::getDNSServerLst() {
        return DNSServerLst;
}

SPtr<TIPv6Addr> TClntIfaceIface::getDsLiteTunnel() {
    return TunnelEndpoint;
}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------
/*
 * just prints important informations (debugging & logging)
 */
std::ostream & operator <<(std::ostream & strum, TClntIfaceIface &x) {
    char buf[48];
    SPtr<TIPv6Addr> addr;
    SPtr<string> str;

    strum << dec;
    strum << "  <ClntIfaceIface";
    strum << " name=\"" << x.Name << "\"";
    strum << " ifindex=\"" << x.ID << "\"";
    strum << " hwType=\"" << x.getHardwareType() << "\"";
    strum << " flags=\"0x" << hex << x.Flags << dec << "\""
          << " mBit=\"" << (x.M_bit_?"1":"0") << "\" oBit=\"" << (x.O_bit_?"1":"0")
          << "\">" << endl;

    strum << "    <!-- " << (x.flagLoopback()?"looback":"no-loopback")
          << (x.flagRunning()?" running":" no-running")
          << (x.flagMulticast()?" multicast -->":" no-multicast -->") << endl;

    strum << "    <!-- " << x.LLAddrCnt << " link scoped addrs -->" << endl;

    for (int i=0; i<x.LLAddrCnt; i++) {
        inet_ntop6(x.LLAddr+i*16,buf);
        strum << "    <Addr>" << buf << "</Addr>" << endl;
    }

    strum << "    <Mac length=\"" << x.Maclen << "\">";
    for (int i=0; i<x.Maclen; i++) {
        strum.fill('0');
        strum.width(2);
        strum << (hex) << (int) x.Mac[i];
        if (i<x.Maclen-1) strum  << ":";
    }
    strum << "</Mac>" << dec << endl;

    SPtr<TIfaceSocket> sock;
    x.firstSocket();
    while (sock = x.getSocket() ) {
        strum << "    " << *sock;
    }
    strum << "    <prefixLength>" << x.getPrefixLength() << "</prefixLength>" << endl;

    strum << "    <!-- options -->" << endl;

    // --- option: DNS-SERVERS ---
    if (!x.DNSServerLst.count()) {
        strum << "    <!-- <dns-servers /> -->" << endl;
    }
    x.DNSServerLst.first();
    while (addr = x.DNSServerLst.get()) {
        strum << "      <dns-server>" << *addr << "</dns-server>" << endl;
    }

    // --- option: DOMAINS ---
    if (!x.DomainLst.count()) {
        strum << "    <!-- <domains /> -->" << endl;
    }
    x.DomainLst.first();
    while (str = x.DomainLst.get()) {
        strum << "      <domain>" << *str << "</domain>" << endl;
    }

    // --- option: NTP-SERVERS ---
    if (!x.NTPServerLst.count()) {
        strum << "    <!-- <ntp-servers /> -->" << endl;
    }
    x.NTPServerLst.first();
    while (addr = x.NTPServerLst.get()) {
        strum << "      <ntp-server>" << *addr << "</ntp-server>" << endl;
    }

    // --- option: TIMEZONE ---
    if (!x.Timezone.length()) {
        strum << "    <!-- <timezone /> -->" << endl;
    } else {
        strum << "    <timezone>" << x.Timezone << "</timezone>" << endl;
    }

    // --- option: SIP-SERVERS ---
    if (!x.SIPServerLst.count()) {
        strum << "    <!-- <sip-servers /> -->" << endl;
    } 
    x.SIPServerLst.first();
    while (addr = x.SIPServerLst.get()) {
        strum << "      <sip-server>" << *addr << "</sip-server>" << endl;
    }

    // --- option: SIP-DOMAINS ---
    if (!x.SIPDomainLst.count()) {
        strum << "    <!-- <sip-domains /> -->" << endl;
    }
    x.SIPDomainLst.first();
    while (str = x.SIPDomainLst.get()) {
        strum << "      <sip-domain>" << *str << "</sip-domain>" << endl;
    }

    // --- option: NIS-SERVERS ---
    if (!x.NISServerLst.count()) {
        strum << "    <!-- <nis-servers /> -->" << endl;
    }
    x.NISServerLst.first();
    while (addr = x.NISServerLst.get()) {
        strum << "      <nis-server>" << *addr << "</nis-server>" << endl;
    }

    // --- option: NIS-DOMAIN ---
    if (!x.NISDomain.length()) {
        strum << "    <!-- <nis-domain /> -->" << endl;
    } else {
        strum << "    <nis-domain>" << x.NISDomain << "</nis-domain>" << endl;
    }

    // --- option: NIS+-SERVERS ---
    if (!x.NISPServerLst.count()) {
        strum << "    <!-- <nisplus-servers /> -->" << endl;
    }
    x.NISPServerLst.first();
    while (addr = x.NISPServerLst.get()) {
        strum << "      <nisplus-server>" << *addr << "</nisplus-server>" << endl;
    }

    // --- option: NIS+-DOMAIN ---
    if (!x.NISPDomain.length()) {
        strum << "    <!-- <nisplus-domain /> -->" << endl;
    } else {
        strum << "    <nisplus-domain>" << x.NISPDomain << "</nisplus-domain>" << endl;
    }

    // --- option: LIFETIME ---
    if (x.LifetimeTimeout==DHCPV6_INFINITY) {
        strum << "    <lifetime timeout=\"INFINITY\" />" << endl;
    } else {
        strum << "    <lifetime timeout=\"" << x.LifetimeTimeout << "\" timestamp=\""
              << x.LifetimeTimestamp << "\" />" << endl;
    }

    strum << "  </ClntIfaceIface>" << endl;
    return strum;
}
