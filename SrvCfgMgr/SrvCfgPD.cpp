/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski
 *          Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "SrvCfgPD.h"
#include "SmartPtr.h"
#include "SrvParsGlobalOpt.h"
#include "DHCPConst.h"
#include "Logger.h"
#include "SrvMsg.h"

using namespace std;

/*
 * static field initialization
 */
unsigned long TSrvCfgPD::StaticID_ = 0;

TSrvCfgPD::TSrvCfgPD()
    :PD_T1Beg_(SERVER_DEFAULT_MIN_T1), PD_T1End_(SERVER_DEFAULT_MAX_T1), 
     PD_T2Beg_(SERVER_DEFAULT_MIN_T2), PD_T2End_(SERVER_DEFAULT_MAX_T2),
     PD_PrefBeg_(SERVER_DEFAULT_MIN_PREF), PD_PrefEnd_(SERVER_DEFAULT_MAX_PREF),
     PD_ValidBeg_(SERVER_DEFAULT_MIN_VALID), PD_ValidEnd_(SERVER_DEFAULT_MAX_VALID)
{
    ID_ = StaticID_++;
    PD_MaxLease_ = SERVER_DEFAULT_CLASSMAXLEASE;
    PD_Assigned_ = 0;
    PD_Count_ = 0;
    PD_Length_ = 0;
}

TSrvCfgPD::~TSrvCfgPD() {
}

unsigned long TSrvCfgPD::chooseTime(unsigned long beg, unsigned long end, unsigned long clntTime)
{
    if (clntTime < beg)
        return beg;
    if (clntTime > end)
        return end;
    return clntTime;
}

unsigned long TSrvCfgPD::getT1(unsigned long hintT1) {
    return chooseTime(PD_T1Beg_, PD_T1End_, hintT1);
}

unsigned long TSrvCfgPD::getT2(unsigned long hintT2) {
    return chooseTime(PD_T2Beg_, PD_T2End_, hintT2);
}

unsigned long TSrvCfgPD::getPrefered(unsigned long hintPref) {
    return chooseTime(PD_PrefBeg_, PD_PrefEnd_, hintPref);
}

unsigned long TSrvCfgPD::getValid(unsigned long hintValid) {
    return chooseTime(PD_ValidBeg_, PD_ValidEnd_, hintValid);
}

unsigned long TSrvCfgPD::getPD_Length() {
    return PD_Length_;
}

bool TSrvCfgPD::setOptions(SPtr<TSrvParsGlobalOpt> opt, int prefixLength)
{
    int poolLength=0;
    Log(Debug) << "PD: Client will receive /" << prefixLength << " prefixes (T1=" << opt->getT1Beg()
               << ".." << opt->getT1End() << ", T2=" << opt->getT2Beg() << ".." << opt->getT2End()
               << ")." <<LogEnd;
    PD_T1Beg_    = opt->getT1Beg();
    PD_T2Beg_    = opt->getT2Beg();
    PD_T1End_    = opt->getT1End();
    PD_T2End_    = opt->getT2End();
    PD_PrefBeg_  = opt->getPrefBeg();
    PD_PrefEnd_  = opt->getPrefEnd();
    PD_ValidBeg_ = opt->getValidBeg();
    PD_ValidEnd_ = opt->getValidEnd();

    PD_Length_   = prefixLength;
    PD_MaxLease_ = opt->getClassMaxLease();

    SPtr<THostRange> PD_Range;

    opt->firstPool();
    SPtr<THostRange> pool;
    if (!(pool=opt->getPool())) {
        Log(Error) << "Unable to find any prefix pools. Please define at least one using 'pd-pool' keyword." << LogEnd;
        return false;
    }
    PD_Count_ = prefixLength - pool->getPrefixLength();
    if (PD_Count_ > 0)
    {
        if (PD_Count_ > 32) {
            PD_Count_ = DHCPV6_INFINITY;
        } else {
            PD_Count_ = ((unsigned long)2) << (PD_Count_ - 1);
        }
    } else {
        PD_Count_ = 1; // only 1 prefix available
    }

    opt->firstPool();
    while ( pool = opt->getPool() ) {
        poolLength = pool->getPrefixLength();
        PoolLst_.append(pool);
        Log(Debug) << "PD: Pool " << pool->getAddrL()->getPlain() << " - "
                   << pool->getAddrR()->getPlain() << ", pool length: "
                   << pool->getPrefixLength() << ", " << PD_Count_ << " prefix(es) total." << LogEnd;
        /** @todo: this code is fishy. It behave erraticaly, when there is only 1 prefix to be assigned
        if (PD_Count_ > pool->rangeCount())
            PD_Count_ = pool->rangeCount();
            cnt++; */
    }

    // calculate common section
    PoolLst_.first();
    pool = PoolLst_.get();
    if (!pool) {
        Log(Crit) << "Unable to find first prefix pool. Something is wrong, very wrong." << LogEnd;
        return false;
    }

    CommonPool_ = new THostRange( new TIPv6Addr(*pool->getAddrL()), new TIPv6Addr(*pool->getAddrR()));
    CommonPool_->truncate(pool->getPrefixLength()+1, prefixLength);
    CommonPool_->setPrefixLength(poolLength);

    /* Log(Debug) << "PD: Common part is " << CommonPool->getAddrL()->getPlain() << " - "
       << CommonPool->getAddrR()->getPlain() << ", pool length: "
       << CommonPool->getPrefixLength() << "." << LogEnd; */

    // set up prefix counter counts
    PD_Assigned_ = 0;
    if (PD_MaxLease_ > PD_Count_)
        PD_MaxLease_ = PD_Count_;
    Log(Debug) << "PD: Up to " << PD_Count_ << " prefixes may be assigned." << LogEnd;
    AllowLst_ = opt->getAllowClientClassString();
    DenyLst_ = opt->getDenyClientClassString();
    return true;
}

bool TSrvCfgPD::prefixInPool(SPtr<TIPv6Addr> prefix)
{
    SPtr<THostRange> pool;
    PoolLst_.first();
    while ( pool = PoolLst_.get() ) {
        if (pool->in(prefix))
            return true;
    }
    return false;
}

/**
 * returns random prefix from a first pool
 *
 * @return
 */
SPtr<TIPv6Addr> TSrvCfgPD::getRandomPrefix()
{
    SPtr<THostRange> pool;
    PoolLst_.first();
    pool = PoolLst_.get();
    if (pool)
        return pool->getRandomPrefix();
    return SPtr<TIPv6Addr>(); // NULL
}

/**
 * gets random prefix from the common part (b) and
 * returns a list of prefixes generated by concatenation
 * of the common part and pool-specific prefix
 *
 * @return list of prefixes (one prefix for each defined pool)
 */
List(TIPv6Addr) TSrvCfgPD::getRandomList() {
    SPtr<TIPv6Addr> commonPart,tmp;
    SPtr<THostRange> range;

    List(TIPv6Addr) lst;
    lst.clear();

    commonPart = CommonPool_->getRandomPrefix();
    commonPart->truncate(0, getPD_Length());

    /// @todo: it's just workaround. Prefix random generation should be implemented for real.
    if (PD_Count_ == PD_Assigned_ + 1) {
        commonPart = new TIPv6Addr(*CommonPool_->getAddrR());
    }

    PoolLst_.first();
    while (range = PoolLst_.get()) {
              tmp = range->getAddrL();
              SPtr<TIPv6Addr> x = new TIPv6Addr(tmp->getAddr(), commonPart->getAddr(),
                                                CommonPool_->getPrefixLength());
              lst.append( x );
    }
    return lst;
}

unsigned long TSrvCfgPD::getPD_MaxLease() {
    return PD_MaxLease_;
}

unsigned long TSrvCfgPD::getID()
{
    return ID_;
}

long TSrvCfgPD::incrAssigned(int count) {
    PD_Assigned_ += count;
    return PD_Assigned_;
}

long TSrvCfgPD::decrAssigned(int count) {
    PD_Assigned_ -= count;
    return PD_Assigned_;
}

unsigned long TSrvCfgPD::getAssignedCount() {
    return PD_Assigned_;
}

unsigned long TSrvCfgPD::getTotalCount() {
    return PD_Count_;
}


ostream& operator<<(ostream& out,TSrvCfgPD& prefix)
{
    out << "    <PD id=\"" << prefix.ID_ << "\">" << std::endl;
    out << "      <!-- total prefixes in class: " << prefix.PD_Count_
        << ", prefixes assigned: " << prefix.PD_Assigned_ << " -->" << endl;
    out << "      <T1 min=\"" << prefix.PD_T1Beg_ << "\" max=\"" << prefix.PD_T1End_  << "\" />" << endl;
    out << "      <T2 min=\"" << prefix.PD_T2Beg_ << "\" max=\"" << prefix.PD_T2End_  << "\" />" << endl;
    out << "      <prefered-lifetime min=\"" << prefix.PD_PrefBeg_ << "\" max=\"" << prefix.PD_PrefEnd_  << "\" />" << endl;
    out << "      <valid-lifetime min=\"" << prefix.PD_ValidBeg_ << "\" max=\"" << prefix.PD_ValidEnd_  << "\" />" << endl;
    out << "      <PDMaxLease>" << prefix.PD_MaxLease_ << "</PDMaxLease>" << endl;

    SPtr<THostRange> statRange;
    out << "      <!-- prefix range -->" << endl;
    SPtr<THostRange> pool;
    prefix.PoolLst_.first();
    while (pool = prefix.PoolLst_.get()) {
        out << *pool;
    }

    /*out << "      <!-- reject-clients ranges:" << addrClass.RejedClnt.count() << " -->" << endl;
    addrClass.RejedClnt.first();
    while(statRange=addrClass.RejedClnt.get())
        out << *statRange;

    out << "      <!-- accept-only ranges:" << addrClass.AcceptClnt.count() << " -->" << endl;
    addrClass.AcceptClnt.first();
    while(statRange=addrClass.AcceptClnt.get())
        out << *statRange;*/
    out << "    </PD>" << std::endl;
    return out;
}



void TSrvCfgPD::mapAllowDenyList( List(TSrvCfgClientClass) clientClassLst)
{
    Log(Info)<<"Mapping allow, deny list to PD  "<< ID_ <<LogEnd;

    SPtr<string> classname;
    SPtr<TSrvCfgClientClass> clntClass;

    AllowLst_.first();
    while (classname = AllowLst_.get()) {
        clientClassLst.first();
        while( clntClass = clientClassLst.get() ) {
            if (clntClass->getClassName()== *classname)
            {
                AllowClientClassLst_.append(clntClass);
                //	Log(Info)<<"  Insert ino allow list  "<<clntClass->getClassName()<<LogEnd;
            }
        }

    }

    DenyLst_.first();
    while (classname = DenyLst_.get())
    {
        clientClassLst.first();
        while( clntClass = clientClassLst.get() )
        {
            if (clntClass->getClassName()== *classname)
            {
                DenyClientClassLst_.append(clntClass);
                //	Log(Info)<<"  Insert ino deny list  "<<clntClass->getClassName()<<LogEnd;
            }
        }

    }
}


bool TSrvCfgPD::clntSupported(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr)
{
    ///@todo implement access control for PD for real
    return true;
}

bool TSrvCfgPD::clntSupported(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr, SPtr<TSrvMsg> msg)
{
    ///@todo implement access control for PD for real

    // is client on denied client class
    SPtr<TSrvCfgClientClass> clntClass;
    DenyClientClassLst_.first();
    while(clntClass = DenyClientClassLst_.get())
    {
        if (clntClass->isStatisfy(msg))
            return false;
    }

    // is client on accepted client class
    AllowClientClassLst_.first();
    while(clntClass = AllowClientClassLst_.get())
    {
        if (clntClass->isStatisfy(msg))
            return true;
    }
    if (AllowClientClassLst_.count())
        return false ;

    return true;
}
