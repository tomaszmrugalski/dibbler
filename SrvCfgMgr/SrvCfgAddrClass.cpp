/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvCfgAddrClass.cpp,v 1.25 2008-10-12 19:42:45 thomson Exp $
 *
 */

#include "SrvCfgAddrClass.h"
#include "SmartPtr.h"
#include "SrvParsGlobalOpt.h"
#include "DHCPConst.h"
#include "Logger.h"
#include "SrvOptAddrParams.h"
#include "SrvMsg.h"

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
    this->ID = staticID++; // client-class ID
    this->AddrsAssigned = 0;
    this->AddrsCount = 0;
}

TSrvCfgAddrClass::~TSrvCfgAddrClass() {
}

/*
 * is client allowed to use this class? (it can be rejected on DUID or address basis)
 */
bool TSrvCfgAddrClass::clntSupported(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr)
{
    SPtr<TStationRange> range;
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
    }

    return true;
}

bool TSrvCfgAddrClass::clntSupported(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr, SPtr<TSrvMsg> msg)
{

    // is client on denied client class
	SPtr<TSrvCfgClientClass> clntClass;
	denyClientClassLst.first();
	while(clntClass = denyClientClassLst.get())
	{
		if (clntClass->isStatisfy(msg))
		return false;
	}

	// is client on accepted client class
	allowClientClassLst.first();
	while(clntClass = allowClientClassLst.get())
	{
		if (clntClass->isStatisfy(msg))
			return true;
	}

    SPtr<TStationRange> range;
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
    }

    if (allowClientClassLst.count())
    	return false ;
   return true;

}
/*
 * is client prefered in this class? (= is it in whitelist?)
 */
bool TSrvCfgAddrClass::clntPrefered(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr)
{
    SPtr<TStationRange> range;
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


long TSrvCfgAddrClass::chooseTime(unsigned long beg, unsigned long end, unsigned long clntTime)
{
    if (clntTime < beg)
	return beg;
    if (clntTime > end)
	return end;
    return clntTime;
}

unsigned long TSrvCfgAddrClass::getT1(unsigned long clntT1) {
    return chooseTime(T1Beg,T1End,clntT1);
}

unsigned long TSrvCfgAddrClass::getT2(unsigned long clntT2) {
    return chooseTime(T2Beg,T2End,clntT2);
}

unsigned long TSrvCfgAddrClass::getPref(unsigned long clntPref) {
    return chooseTime(PrefBeg,PrefEnd,clntPref);
}

unsigned long TSrvCfgAddrClass::getValid(unsigned long clntValid) {
    return chooseTime(ValidBeg,ValidEnd,clntValid);
}

void TSrvCfgAddrClass::setOptions(SPtr<TSrvParsGlobalOpt> opt)
{
    T1Beg    = opt->getT1Beg();
    T2Beg    = opt->getT2Beg();
    T1End    = opt->getT1End();
    T2End    = opt->getT2End();
    PrefBeg  = opt->getPrefBeg();
    PrefEnd  = opt->getPrefEnd();
    ValidBeg = opt->getValidBeg();
    ValidEnd = opt->getValidEnd();
    Share    = opt->getShare();

    allowLst = opt->getAllowClientClassString();
    denyLst = opt->getDenyClientClassString();

    ClassMaxLease = opt->getClassMaxLease();

    SPtr<TStationRange> statRange;
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

    AddrParams = opt->getAddrParams();
}

bool TSrvCfgAddrClass::addrInPool(SPtr<TIPv6Addr> addr)
{
    return Pool->in(addr);
}

unsigned long TSrvCfgAddrClass::countAddrInPool()
{
    return this->AddrsCount;
}

SPtr<TIPv6Addr> TSrvCfgAddrClass::getRandomAddr()
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

unsigned long TSrvCfgAddrClass::getShare() {
    return this->Share;
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
    SPtr<TIPv6Addr> addr = new TIPv6Addr("fe80::",true);
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

SPtr<TSrvOptAddrParams> TSrvCfgAddrClass::getAddrParams()
{
    return AddrParams;
}


ostream& operator<<(ostream& out,TSrvCfgAddrClass& addrClass)
{
    out << "    <class id=\"" << addrClass.ID << "\" share=\"" << addrClass.Share << "\">" << std::endl;
    out << "      <!-- total addrs in class: " << addrClass.AddrsCount
	<< ", addrs assigned: " << addrClass.AddrsAssigned << " -->" << endl;
    out << "      <T1 min=\"" << addrClass.T1Beg << "\" max=\"" << addrClass.T1End  << "\" />" << endl;
    out << "      <T2 min=\"" << addrClass.T2Beg << "\" max=\"" << addrClass.T2End  << "\" />" << endl;
    out << "      <pref min=\"" << addrClass.PrefBeg << "\" max=\""<< addrClass.PrefEnd  << "\" />" <<endl;
    out << "      <valid min=\"" << addrClass.ValidBeg << "\" max=\""<< addrClass.ValidEnd << "\" />" << endl;
    out << "      <ClassMaxLease>" << addrClass.ClassMaxLease << "</ClassMaxLease>" << endl;

    SPtr<TStationRange> statRange;
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

    if (addrClass.AddrParams)
	out << "      <AddrParams prefix=\"" << addrClass.AddrParams->getPrefix() << "\" bitfield=\""
	    << addrClass.AddrParams->getBitfield() << "\"/>" << endl;
    out << "    </class>" << std::endl;
    return out;
}

/** 
 * Create the AllowClientClassLst and DenyClientClassLst
 * 
 * @param clientClassLst list of available client class names
 */
void TSrvCfgAddrClass::mapAllowDenyList( List(TSrvCfgClientClass) clientClassLst )
{

    Log(Info) << "Mapping allow, deny list to class "<< ID << ":" << clientClassLst.count() << " allow/deny entries in total." << LogEnd;

    SPtr<string> classname;
    SPtr<TSrvCfgClientClass> clntClass;
    
    allowLst.first();
    while (classname = allowLst.get())
    {
	clientClassLst.first();
	while( clntClass = clientClassLst.get() )
	{
	    if (clntClass->getClassName()== *classname)
	    {
		allowClientClassLst.append(clntClass);
		Log(Debug) << "  Insert into allow list " <<clntClass->getClassName() << LogEnd;
	    }
	}
    }
    
    denyLst.first();
    while (classname = denyLst.get())
    {
	clientClassLst.first();
	while( clntClass = clientClassLst.get() )
	{
	    if (clntClass->getClassName()== *classname)
	    {
		denyClientClassLst.append(clntClass);
		Log(Debug) << "  Insert into deny list " <<clntClass->getClassName()<<LogEnd;
	    }
	}
    }
}

