/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvCfgTA.cpp,v 1.1.2.1 2006-02-05 23:42:33 thomson Exp $
 */

#include "SrvCfgTA.h"
#include "SmartPtr.h"
#include "SrvParsGlobalOpt.h"
#include "DHCPConst.h"
#include "Logger.h"

/*
 * static field initialization
 */
unsigned long TSrvCfgTA::staticID=0;

 
TSrvCfgTA::TSrvCfgTA() {
    this->Pref  = SERVER_DEFAULT_TA_PREF_LIFETIME;
    this->Pref  = SERVER_DEFAULT_TA_VALID_LIFETIME;
    this->ID    = staticID++;
    this->AddrsAssigned = 0;
    this->AddrsCount = 0;
}

TSrvCfgTA::~TSrvCfgTA() {
}

/** 
 * is client allowed to use this class? (it can be rejected on DUID or address basis)
 * 
 * @param clntDuid 
 * @param clntAddr 
 * 
 * @return 
 */bool TSrvCfgTA::clntSupported(SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr)
{
    SmartPtr<TStationRange> range;
    RejedClnt.first();
    // is client on black list?
    while(range=RejedClnt.get())
        if (range->in(clntDuid,clntAddr))
            return false;
    
    if (AcceptClnt.count()) {
        AcceptClnt.first();
	    // there's white list
        while(range=AcceptClnt.get()) {
	    // is client on this white list?
            if (range->in(clntDuid,clntAddr))
                return true;
        }
        return false;
    } 
      
    return true;
}

/*
 * is client prefered in this class? (= is it in whitelist?)
 */
bool TSrvCfgTA::clntPrefered(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr)
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
        
    } 
    return false;
}

unsigned long TSrvCfgTA::getPref() {
    return this->Pref;
}

unsigned long TSrvCfgTA::getValid() {
    return this->Valid;
}

void TSrvCfgTA::setOptions(SmartPtr<TSrvParsGlobalOpt> opt)
{
    if (opt->getPrefBeg()!=opt->getPrefEnd()) {
	Log(Warning) << "TA-class does not support preferred-lifetime ranges. Lower bound ("
		     << opt->getPrefBeg() << ") was used." << LogEnd;
    }
    this->Pref = opt->getPrefBeg();

    if (opt->getValidBeg()!=opt->getValidEnd()) {
	Log(Warning) << "TA-class does not support valid-lifetime ranges. Lower bound ("
		     << opt->getPrefBeg() << ") was used." << LogEnd;
    }
    this->Valid = opt->getValidBeg();

    ClassMaxLease = opt->getClassMaxLease();

    // copy black-list
    SmartPtr<TStationRange> statRange;
    opt->firstRejedClnt();
    while(statRange=opt->getRejedClnt())
        this->RejedClnt.append(statRange);

    // copy white-list
    opt->firstAcceptClnt();
    while(statRange=opt->getAcceptClnt())
        this->AcceptClnt.append(statRange);
	
    opt->firstPool();
    this->Pool = opt->getPool();
    if (opt->getPool()) {
	Log(Warning) << "Two or more pool defined for TA. Only one is used." << LogEnd;
    }

    // set up address counter counts
    this->AddrsCount = this->Pool->rangeCount();
    this->AddrsAssigned = 0;

    if (this->ClassMaxLease > this->AddrsCount)
	this->ClassMaxLease = this->AddrsCount;
}

unsigned long TSrvCfgTA::countAddrInPool()
{
    return this->AddrsCount;
}

SmartPtr<TIPv6Addr> TSrvCfgTA::getRandomAddr()
{
    return Pool->getRandomAddr();
}

unsigned long TSrvCfgTA::getClassMaxLease() {
    return ClassMaxLease;
}

unsigned long TSrvCfgTA::getID()
{
    return this->ID;
}

long TSrvCfgTA::incrAssigned(int count) {
    this->AddrsAssigned += count;
    return this->AddrsAssigned;
}

long TSrvCfgTA::decrAssigned(int count) {
    this->AddrsAssigned -= count;
    return this->AddrsAssigned;
}

unsigned long TSrvCfgTA::getAssignedCount() {
    return this->AddrsAssigned;
}

bool TSrvCfgTA::addrInPool(SmartPtr<TIPv6Addr> addr) {
    // FIXME: Implement this or TA won't work in CONFIRM
    Log(Crit) << "TA support is not entirely implemented." << LogEnd;
    return false;
}

ostream& operator<<(ostream& out,TSrvCfgTA& addrClass)
{
    out << "    <taClass id=\"" << addrClass.ID << "\" pref=\"" << addrClass.Pref 
	<< "\" valid=\"" << addrClass.Valid << "\"  />" << endl;
    out << "      <!-- total addrs in class: " << addrClass.AddrsCount 
	<< ", addrs assigned: " << addrClass.AddrsAssigned << " -->" << endl;
    out << "      <ClassMaxLease>" << addrClass.ClassMaxLease << "</ClassMaxLease>" << endl;
        
    SmartPtr<TStationRange> statRange;
    out << "      <!-- address range -->" << endl;
    out << *addrClass.Pool;
    
    out << "      <!-- reject-clients (black list) ranges:" << addrClass.RejedClnt.count() << " -->" << endl;
    addrClass.RejedClnt.first();
    while(statRange=addrClass.RejedClnt.get())
	out << *statRange;	
	
    out << "      <!-- accept-only (white list) ranges:" << addrClass.AcceptClnt.count() << " -->" << endl;
    addrClass.AcceptClnt.first();
    while(statRange=addrClass.AcceptClnt.get())
	out << *statRange;
    out << "    </taClass>" << std::endl;
    return out;
}

/*
 * $Log: not supported by cvs2svn $
 */
