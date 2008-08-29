/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptVendorSpec.cpp,v 1.6 2008-08-29 00:07:29 thomson Exp $
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
    SmartPtr<TClntIfaceMgr> ifaceMgr = msg->getClntIfaceMgr();
    SmartPtr<TClntIfaceIface> iface = (Ptr*)ifaceMgr->getIfaceByID(msg->getIface());

    if (!iface) {
	Log(Error) << "Unable to find interface with ifindex=" << msg->getIface() << LogEnd;
	return false;
    }

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
	    tunnelType = buf[pos];
	    break;
	}
	case OPTION_VENDORSPEC_ENDPOINT:
	{
	    tunnelEndpoint = new TIPv6Addr(buf+pos);
	    break;
	}
	default:
	    Log(Warning) << "Unknown vendor-spec suboption: " << code << ", ignored." << LogEnd;
	}

	pos += length;
    }

    if (tunnelType != -1) {
	Log(Info) << "Tunnel: Tunnel type set to " << tunnelType << LogEnd;
    }
    if (tunnelEndpoint) {
	Log(Info) << "Tunnel: Tunnel endpoint is " << tunnelEndpoint->getPlain() << LogEnd;
    }

    // perform tunnel-mode operations
    iface->setTunnelMode(tunnelType, tunnelEndpoint);
    return true;
}

void TClntOptVendorSpec::setIfaceMgr(SPtr<TClntIfaceMgr> ifaceMgr)
{
    this->IfaceMgr = IfaceMgr;
}
