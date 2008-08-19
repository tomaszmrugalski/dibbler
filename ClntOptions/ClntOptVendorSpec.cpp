/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptVendorSpec.cpp,v 1.4 2008-08-19 00:09:39 thomson Exp $
 *
 */

#include "Logger.h"
#include "ClntOptVendorSpec.h"

TClntOptVendorSpec::TClntOptVendorSpec( char * buf,  int n, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_VENDOR_OPTS, buf,n, parent)
{
}

TClntOptVendorSpec::TClntOptVendorSpec(int enterprise, char * data, int dataLen, TMsg* parent)
    :TOptVendorSpecInfo(OPTION_VENDOR_OPTS, enterprise, data, dataLen, parent)
{
}

bool TClntOptVendorSpec::doDuties() {
    TClntMsg * msg = dynamic_cast<TClntMsg*>(Parent);
    SmartPtr<TClntCfgMgr> cfgMgr = msg->getClntCfgMgr();

    if (Vendor != cfgMgr->tunnelMode())
    {
	Log(Debug) << "Tunnel-mode: vendor expected=" << cfgMgr->tunnelMode() << ", received=" << Vendor << LogEnd;
	return true;
    }

    int tunnelType = -1;
    SPtr<TIPv6Addr> tunnelEndpoint = 0;

    int pos=0;
    char * buf = VendorData;
    int bufSize = VendorDataLen;
    while (pos<VendorDataLen)
    {
	if (pos+4>bufSize)
	{
	    Log(Error) << "Vendor-spec option truncated. There are " << (bufSize-pos) 
		       << " bytes left to parse. Bytes ignored." << LogEnd;
	}
        unsigned short code   = buf[pos]*256 + buf[pos+1];
        pos+=2;
        unsigned short length = buf[pos]*256 + buf[pos+1];
        pos+=2;

	if (pos+length>bufSize)
	{
	    Log(Error) << "Invalid suboption in vendor-spec option (type=" << code 
		       << ", len=" << length << ")." << LogEnd;
	}
	switch (code)
	{
	case OPTION_VENDORSPEC_TUNNEL_TYPE:
	{
	    Log(Debug) << "#### TunnelType found" << LogEnd;
	    tunnelType = buf[pos];
	    break;
	}
	case OPTION_VENDORSPEC_ENDPOINT:
	{
	    Log(Debug) << "#### TunnelEndpoint found" << LogEnd;
	    tunnelEndpoint = new TIPv6Addr(buf+pos);
	    break;
	}
	default:
	    Log(Warning) << "Unknown vendor-spec suboption: " << code << ", ignored." << LogEnd;
	}

	pos += length;
    }

    // perform tunnel-mode operations
    Log(Debug) << "#### tunnel-mode=" << tunnelType << ", endpoint=" << tunnelEndpoint->getPlain() << LogEnd;
    return true;
}

void TClntOptVendorSpec::setIfaceMgr(SPtr<TClntIfaceMgr> ifaceMgr)
{
    this->IfaceMgr = IfaceMgr;
}
