/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 */

#include "NodeClientSpecific.h"
#include "SrvMsg.h"
#include "DHCPConst.h"
#include "OptVendorData.h"
#include "OptVendorClass.h"
#include "hex.h"
#include <sstream>

using namespace std;

SPtr<TSrvMsg> NodeClientSpecific::CurrentMsg;
string NodeClientSpecific::vendor_spec_num  ;
string NodeClientSpecific::vendor_spec_data ;
string NodeClientSpecific::vendor_class_num ;
string NodeClientSpecific::vendor_class_data ;

NodeClientSpecific::NodeClientSpecific()
    :Node(NODE_CLIENT_SPECIFIC)
{
    CurrentMsg.reset();
    Type = CLIENT_UNKNOWN;
}

NodeClientSpecific::~NodeClientSpecific() {
}

string NodeClientSpecific::exec(SPtr<TSrvMsg> msg)
{

    // If have not analyse the Msg, then analyse it
    if (CurrentMsg != msg)
    {
	analyseMessage(msg);
    } // if new message


    switch (Type)
    {
    case NodeClientSpecific::CLIENT_VENDOR_SPEC_ENTERPRISE_NUM :
	return vendor_spec_num;
	break;
    case NodeClientSpecific::CLIENT_VENDOR_SPEC_DATA :
	return vendor_spec_data;
	break;
    case NodeClientSpecific::CLIENT_VENDOR_CLASS_ENTERPRISE_NUM :
	return vendor_class_num;
	break;
    case NodeClientSpecific::CLIENT_VENDOR_CLASS_DATA :
	return vendor_class_data;
	break;
    default :
	return "";
    }
}

NodeClientSpecific::NodeClientSpecific(ClientSpecificType t)
    :Node(NODE_CLIENT_SPECIFIC)
{
    CurrentMsg.reset();
    Type = t;
}

void  NodeClientSpecific::analyseMessage(SPtr<TSrvMsg> msg)
{
    if (CurrentMsg == msg)
	return;

    CurrentMsg = msg;
    vendor_spec_num = "";
    vendor_spec_data = "";
    vendor_class_num = "";
    vendor_class_data = "";

    SPtr<TOpt> ptrOpt;

    msg->firstOption();

    while (ptrOpt = msg->getOption())	{
	switch (ptrOpt->getOptType()) {
	case OPTION_VENDOR_OPTS:
	{
	    SPtr<TOptVendorSpecInfo> vendorspec = (Ptr*) ptrOpt;
	    stringstream convert;

	    // convert enterprise-id to string
	    convert << vendorspec->getVendor();
	    convert >> vendor_spec_num;

	    // now the tricky part: convert content

#if 1
	    // The following text converts content of the vendor options
	    // into string as is (this will produce junk if the content is
	    // not ASCII printable
	    vendor_spec_data = "";
	    vendorspec->firstOption();
	    while (SPtr<TOpt> opt = vendorspec->getOption()) {
		int len = opt->getSize();
		char* buf = new char[len+1];
		buf[len] = 0; // make sure it is null-terminated
		opt->storeSelf(buf);
		vendor_spec_data += string(buf + 4); // +4 (skip packet header)
		delete [] buf;
	    }
#else
	    // The following code converts option into hex string, eg.
	    // 00:01:00:05:48:49:4a:4b:4c
	    int len = vendorspec->getSize();
	    char* buf = new char[len+1];
	    buf[len]=0;
	    vendorspec->storeSelf(buf);
	    vendor_spec_data = hexToText((uint8_t*)buf + 8, len - 8, true, false);
	    delete [] buf;
#endif
	    break;
	}

	case OPTION_VENDOR_CLASS:
	    SPtr<TOptVendorClass> vendorclass = (Ptr*) ptrOpt;

	    // Convert enterprise-id
	    stringstream convert;
	    convert << vendorclass->Enterprise_id_;
	    convert >> vendor_class_num;

	    // Convert content of all sub-options
	    vendor_class_data = "";
	    for (std::vector<TOptUserClass::UserClassData>::const_iterator data =
		     vendorclass->userClassData_.begin();
		 data != vendorclass->userClassData_.end(); ++data) {
		vendor_class_data += std::string(
		    reinterpret_cast<const char*>(&data->opaqueData_[0]),
		    data->opaqueData_.size());
	    }

	    break;

	} // switch
    } // while
}
