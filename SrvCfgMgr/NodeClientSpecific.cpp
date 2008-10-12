/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: NodeClientSpecific.cpp,v 1.1 2008-10-12 19:36:58 thomson Exp $
 *
 */

#include "NodeClientSpecific.h"
#include "SrvMsg.h"
#include "DHCPConst.h"
#include "SrvOptVendorClass.h"
#include <sstream>

SmartPtr<TSrvMsg> NodeClientSpecific::CurrentMsg;
string NodeClientSpecific::vendor_spec_num  ;
string NodeClientSpecific::vendor_spec_data ;
string NodeClientSpecific::vendor_class_num ;
string NodeClientSpecific::vendor_class_data ;

NodeClientSpecific::NodeClientSpecific() 
    :Node(NODE_CLIENT_SPECIFIC)
{
    CurrentMsg = 0;
    Type = CLIENT_UNKNOWN;
}

NodeClientSpecific::~NodeClientSpecific() {
}

string NodeClientSpecific::exec(SmartPtr<TSrvMsg> msg)
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
    CurrentMsg = 0;
    Type = t;
}

void  NodeClientSpecific::analyseMessage(SmartPtr<TSrvMsg> msg)
{
    if (CurrentMsg != msg)
    {
	CurrentMsg = msg;
	
	SmartPtr<TOpt> ptrOpt;
	SmartPtr<TOpt> ptrOpt2;
	
	stringstream convert;
	msg->firstOption();
	
	SPtr<TSrvOptVendorSpec> vendorspec;
	SPtr<TSrvOptVendorSpec> vendorclass;
	//SPtr<TSrvOptVendorSpec> vendorclass;
	
	while (ptrOpt = msg->getOption())	{
	    switch (ptrOpt->getOptType()) {
	    case OPTION_VENDOR_OPTS:
		vendorspec = (Ptr*) ptrOpt;
		convert<< vendorspec->getVendor();
		convert>>vendor_spec_num;
		vendor_spec_data = vendorspec->getVendorData();
		
		break;
		
	    case OPTION_VENDOR_CLASS:
		vendorclass =  (Ptr*) ptrOpt2;
		convert<< vendorclass->getVendor();
		convert>>vendor_class_num;
		vendor_class_data = vendorclass->getVendorData();
		break;
		
	    } // switch
	} // while
	
    } // if new message
}
