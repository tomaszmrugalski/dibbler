/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 */

class TSrvParsClassOpt;
#ifndef TSRVPARSCLASS_H
#define TSRVPARSCLASS_H

#include <string>
#include "DHCPConst.h"
#include "Container.h"
#include "SmartPtr.h"
#include "HostID.h"
#include "HostRange.h"
#include "IPv6Addr.h"
#include "SrvOptAddrParams.h"

class TSrvParsClassOpt
{
 public:
    TSrvParsClassOpt(void);
    ~TSrvParsClassOpt(void);

    //T1,T2,Valid,Prefered time routines
    void setT1Beg(unsigned long t1);
    void setT1End(unsigned long t1);
    unsigned long getT1Beg();
    unsigned long getT1End();

    void setT2Beg(unsigned long t2);
    void setT2End(unsigned long t2);
    unsigned long getT2Beg();
    unsigned long getT2End();

    void setPrefBeg(unsigned long pref);
    void setPrefEnd(unsigned long pref);
    unsigned long getPrefBeg();
    unsigned long getPrefEnd();

    void setShare(unsigned long share);
    unsigned long getShare();

    void setValidEnd(unsigned long valid);
    void setValidBeg(unsigned long valid);
    unsigned long getValidEnd();
    unsigned long getValidBeg();

    //Rejected clients access routines
    void addRejedClnt(SPtr<THostRange> addr);
    void firstRejedClnt();
    SPtr<THostRange> getRejedClnt();
    void setRejedClnt(TContainer<SPtr<THostRange> > *rejedClnt);

    //Accepted clients access routines
    void addAcceptClnt(SPtr<THostRange> addr);
    void firstAcceptClnt();
    SPtr<THostRange> getAcceptClnt();
    void setAcceptClnt(TContainer<SPtr<THostRange> > *acceptClnt);

    //Pool access routines
    void addPool(SPtr<THostRange> addr);
    void firstPool();
    SPtr<THostRange> getPool();
    void setPool(TContainer<SPtr<THostRange> > *pool);
    long countPool();

    // leases count
    void setClassMaxLease(unsigned long maxClntLeases);
    unsigned long getClassMaxLease();

    void setAddrParams(int prefix, int bitfield);
    SPtr<TSrvOptAddrParams> getAddrParams();

    // Allow and deny list

    void setAllowClientClass(std::string s);
    List(std::string) getAllowClientClassString();

    void setDenyClientClass(std::string s);
    List(std::string) getDenyClientClassString();

private:
    //Ranges of T1 i T2
    unsigned long T1Beg;
    unsigned long T1End;
    unsigned long T2End;
    unsigned long T2Beg;
    unsigned long PrefBeg;
    unsigned long PrefEnd;
    unsigned long ValidBeg;
    unsigned long ValidEnd;
    unsigned long Share;

    TContainer<SPtr<THostRange> > RejedClnt;
    TContainer<SPtr<THostRange> > AcceptClnt;
    TContainer<SPtr<THostRange> > Pool;

    unsigned long ClassMaxLease;

    // AddrParams fields
    SPtr<TSrvOptAddrParams> AddrParams;

    List(std::string) allowLst;
    List(std::string) denyLst;
};
#endif
