/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgAddrClass.cpp,v 1.20 2005-08-02 00:33:58 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.19  2005/03/15 00:36:22  thomson
 * 0.4.0 release (win32 commit)
 *
 * Revision 1.18  2004/12/07 00:43:03  thomson
 * Server no longer support link local addresses (bug #38),
 * Server now supports stateless mode (bug #71)
 *
 * Revision 1.17  2004/12/03 20:51:42  thomson
 * Logging issues fixed.
 *
 * Revision 1.16  2004/10/27 22:07:56  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.15  2004/09/08 21:22:45  thomson
 * Parser improvements, signed/unsigned issues addressed.
 *
 * Revision 1.14  2004/09/03 23:20:23  thomson
 * RAPID-COMMIT support fixed. (bugs #50, #51, #52)
 *
 *
 * Revision 1.12  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 * Revision 1.11  2004/06/29 22:03:36  thomson
 * *MaxLease option partialy implemented/fixed.
 *
 * Revision 1.10  2004/06/28 22:37:59  thomson
 * Minor changes.
 *
 * Revision 1.9  2004/06/28 21:34:18  thomson
 * DUID is now parsed properly and SrvCfgMgr dumps valid xml file.
 *
 * Revision 1.8  2004/06/20 19:29:23  thomson
 * New address assignment finally works.
 *
 * Revision 1.7  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
 *                                                                           
 */

#include "SrvCfgAddrClass.h"
#include "SmartPtr.h"
#include "SrvParsGlobalOpt.h"
#include "DHCPConst.h"
#include "Logger.h"

/*
 * static field initialization
 */
unsigned long TSrvCfgAddrClass::staticID=0;

 
TSrvCfgAddrClass::TSrvCfgAddrClass() {
    this->T1Beg    = 0;
    this->T1End    = DHCPV6_INFINITY;
    this->T2Beg    = 0;
    this->T2End    = DHCPV6_INFINITY;
    this->PrefBeg  = 0;
    this->PrefEnd  = DHCPV6_INFINITY;
    this->ValidBeg = 0;
    this->ValidEnd = DHCPV6_INFINITY;
    this->ID = staticID++;
    this->AddrsAssigned = 0;
    this->AddrsCount = 0;
}

TSrvCfgAddrClass::~TSrvCfgAddrClass() {
}

/*
 * is client allowed to use this class? (it can be rejected on DUID or address basis)
 */
bool TSrvCfgAddrClass::clntSupported(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr)
{
    SmartPtr<TStationRange> range;
    RejedClnt.first();
    // is client on black list?
    while(range=RejedClnt.get())
        if (range->in(duid,clntAddr))
            return false;
    
    if (AcceptClnt.count()) {
        AcceptClnt.first();
	    // there's white list
        while(range=AcceptClnt.get()) {
	    // is client on this white list?
            if (range->in(duid,clntAddr))
                return true;
        }
        return false;
    } else {
        return true;
    }
}

/*
 * is client prefered in this class? (= is it in whitelist?)
 */
bool TSrvCfgAddrClass::clntPrefered(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr)
{
    SmartPtr<TStationRange> range;
    RejedClnt.first();
    // is client on black list?
    while(range=RejedClnt.get())
        if (range->in(duid,clntAddr))
            return false;
            
    if (AcceptClnt.count()) {
        AcceptClnt.first();
        while(range=AcceptClnt.get()) {
            if (range->in(duid,clntAddr))
                return true;
        }
        return false;
    } else {
        return false;
    }
}


long TSrvCfgAddrClass::chooseTime(long beg, long end, long clntTime)
{
    if (clntTime < beg)
	return beg;
    if (clntTime > end)
	return end;
    return clntTime;
}

unsigned long TSrvCfgAddrClass::getT1(long clntT1) {
    return chooseTime(T1Beg,T1End,clntT1);
}

unsigned long TSrvCfgAddrClass::getT2(long clntT2) {
    return chooseTime(T2Beg,T2End,clntT2);
}

unsigned long TSrvCfgAddrClass::getPref(long clntPref) {
    return chooseTime(PrefBeg,PrefEnd,clntPref);
}

unsigned long TSrvCfgAddrClass::getValid(long clntValid) {
    return chooseTime(ValidBeg,ValidEnd,clntValid);
}

void TSrvCfgAddrClass::setOptions(SmartPtr<TSrvParsGlobalOpt> opt)
{
    T1Beg=opt->getT1Beg();
    T2Beg=opt->getT2Beg();
    T1End=opt->getT1End();
    T2End=opt->getT2End();
    PrefBeg=opt->getPrefBeg();
    PrefEnd=opt->getPrefEnd();
    ValidBeg=opt->getValidBeg();
    ValidEnd=opt->getValidEnd();
    
    ClassMaxLease = opt->getClassMaxLease();
    
    SmartPtr<TStationRange> statRange;
    opt->firstRejedClnt();
    while(statRange=opt->getRejedClnt())
        this->RejedClnt.append(statRange);
    
    opt->firstAcceptClnt();
    while(statRange=opt->getAcceptClnt())
        this->AcceptClnt.append(statRange);
	
    opt->firstPool();
    this->Pool = opt->getPool();
    if (opt->getPool()) {
	Log(Warning) << "Two or more pool defined. Only one is used." << LogEnd;
    }

    // set up address counter counts
    this->AddrsCount = this->Pool->rangeCount();
    this->AddrsAssigned = 0;

    if (this->ClassMaxLease > this->AddrsCount)
	this->ClassMaxLease = this->AddrsCount;
}

bool TSrvCfgAddrClass::addrInPool(SmartPtr<TIPv6Addr> addr)
{
    return Pool->in(addr);
}

unsigned long TSrvCfgAddrClass::countAddrInPool()
{
    return this->AddrsCount;
}

SmartPtr<TIPv6Addr> TSrvCfgAddrClass::getRandomAddr()
{
    return Pool->getRandomAddr();
}

unsigned long TSrvCfgAddrClass::getClassMaxLease() {
    return ClassMaxLease;
}

unsigned long TSrvCfgAddrClass::getID()
{
    return this->ID;
}

long TSrvCfgAddrClass::incrAssigned(int count) {
    this->AddrsAssigned += count;
    return this->AddrsAssigned;
}

long TSrvCfgAddrClass::decrAssigned(int count) {
    this->AddrsAssigned -= count;
    return this->AddrsAssigned;
}

unsigned long TSrvCfgAddrClass::getAssignedCount() {
    return this->AddrsAssigned;
}

bool TSrvCfgAddrClass::isLinkLocal() {
    SmartPtr<TIPv6Addr> addr = new TIPv6Addr("fe80::",true);
    if (this->addrInPool(addr)) {
	Log(Crit) << "Link local address (fe80::) belongs to the class." << LogEnd;
	return true;
    }

    addr = new TIPv6Addr("fe80:ffff:ffff:ffff:ffff:ffff:ffff:ffff", true);
    if (this->addrInPool(addr)) {
	Log(Crit) << "Link local address (fe80:ffff:ffff:ffff:ffff:ffff:ffff:ffff) belongs to the class." << LogEnd;
	return true;
    }

    addr = this->Pool->getAddrL();
    char linklocal[] = { 0xfe, 0x80};

    if (!memcmp(addr->getAddr(), linklocal,2)) {
	Log(Crit) << "Staring address " << addr->getPlain() << " is link-local." << LogEnd;
	return true;
    }
    
    addr = this->Pool->getAddrR();
    if (!memcmp(addr->getAddr(), linklocal,2)) {
	Log(Crit) << "Ending address " << addr->getPlain() << " is link-local." << LogEnd;
	return true;
    }

    return false;
}

ostream& operator<<(ostream& out,TSrvCfgAddrClass& addrClass)
{
    out << "    <class id=\"" << addrClass.ID << "\">" << std::endl;
    out << "      <!-- total addrs in class: " << addrClass.AddrsCount 
	<< ", addrs assigned: " << addrClass.AddrsAssigned << " -->" << endl;
    out << "      <T1 min=\"" << addrClass.T1Beg << "\" max=\"" << addrClass.T1End  << "\" />" << endl;
    out << "      <T2 min=\"" << addrClass.T2Beg << "\" max=\"" << addrClass.T2End  << "\" />" << endl;
    out << "      <pref min=\"" << addrClass.PrefBeg << "\" max=\""<< addrClass.PrefEnd  << "\" />" <<endl;
    out << "      <valid min=\"" << addrClass.ValidBeg << "\" max=\""<< addrClass.ValidEnd << "\" />" << endl;
    out << "      <ClassMaxLease>" << addrClass.ClassMaxLease << "</ClassMaxLease>" << endl;
        
    SmartPtr<TStationRange> statRange;
    out << "      <!-- address range -->" << endl;
    out << *addrClass.Pool;
    
    out << "      <!-- reject-clients ranges:" << addrClass.RejedClnt.count() << " -->" << endl;
    addrClass.RejedClnt.first();
    while(statRange=addrClass.RejedClnt.get())
	out << *statRange;	
	
    out << "      <!-- accept-only ranges:" << addrClass.AcceptClnt.count() << " -->" << endl;
    addrClass.AcceptClnt.first();
    while(statRange=addrClass.AcceptClnt.get())
	out << *statRange;
    out << "    </class>" << std::endl;
    return out;
}
