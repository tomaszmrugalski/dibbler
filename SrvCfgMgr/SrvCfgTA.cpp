/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 * changes: Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvCfgTA.cpp,v 1.5 2008-10-12 20:07:31 thomson Exp $
 */

#include "SrvCfgTA.h"
#include "SmartPtr.h"
#include "SrvParsGlobalOpt.h"
#include "DHCPConst.h"
#include "Logger.h"
#include "SrvMsg.h"
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
 */
bool TSrvCfgTA::clntSupported(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr)
{
    SPtr<TStationRange> range;
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


 bool TSrvCfgTA::clntSupported(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr, SPtr<TSrvMsg> msg)
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
bool TSrvCfgTA::clntPrefered(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr)
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

    }
    return false;
}

unsigned long TSrvCfgTA::getPref() {
    return this->Pref;
}

unsigned long TSrvCfgTA::getValid() {
    return this->Valid;
}

void TSrvCfgTA::setOptions(SPtr<TSrvParsGlobalOpt> opt)
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
    SPtr<TStationRange> statRange;
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

    // Get ClientClass

    allowLst = opt->getAllowClientClassString();
    denyLst = opt->getDenyClientClassString();

}

unsigned long TSrvCfgTA::countAddrInPool()
{
    return this->AddrsCount;
}

SPtr<TIPv6Addr> TSrvCfgTA::getRandomAddr()
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

bool TSrvCfgTA::addrInPool(SPtr<TIPv6Addr> addr) 
{
    return Pool->in(addr);
}

ostream& operator<<(ostream& out,TSrvCfgTA& addrClass)
{
    out << "    <taClass id=\"" << addrClass.ID << "\" pref=\"" << addrClass.Pref
	<< "\" valid=\"" << addrClass.Valid << "\">" << endl;
    out << "      <!-- total addrs in class: " << addrClass.AddrsCount
	<< ", addrs assigned: " << addrClass.AddrsAssigned << " -->" << endl;
    out << "      <ClassMaxLease>" << addrClass.ClassMaxLease << "</ClassMaxLease>" << endl;

    SPtr<TStationRange> statRange;
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


void TSrvCfgTA::mapAllowDenyList( List(TSrvCfgClientClass) clientClassLst)
{

	Log(Info)<<"Mapping allow, deny list to TA "<< ID <<LogEnd;

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
			//	Log(Info)<<"  Insert ino allow list  "<<clntClass->getClassName()<<LogEnd;
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
				//	Log(Info)<<"  Insert ino deny list  "<<clntClass->getClassName()<<LogEnd;
				}
			}

		}
}


/*
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2008-08-29 00:07:33  thomson
 * Temporary license change(GPLv2 or later -> GPLv2 only)
 *
 * Revision 1.3  2006-03-21 19:12:47  thomson
 * TA related tune ups.
 *
 * Revision 1.2  2006/03/05 21:34:05  thomson
 * Temp. addresses support merged to the top branch.
 *
 * Revision 1.1.2.1  2006/02/05 23:42:33  thomson
 * Initial revision.
 *
 */
