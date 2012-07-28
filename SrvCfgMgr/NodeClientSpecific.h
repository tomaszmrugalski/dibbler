/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 */

#ifndef NODECLIENTSPECIFIC_H_
#define NODECLIENTSPECIFIC_H_

#include "Node.h"
#include "SmartPtr.h"
#include "Container.h"
#include "Opt.h"
#include <iostream>

class NodeClientSpecific: public Node {

public:
    enum ClientSpecificType
    {
	CLIENT_UNKNOWN = 0,
	CLIENT_VENDOR_SPEC_ENTERPRISE_NUM = 1,
	CLIENT_VENDOR_SPEC_DATA = 2,
	CLIENT_VENDOR_CLASS_ENTERPRISE_NUM = 3,
	CLIENT_VENDOR_CLASS_DATA = 4
    };

    NodeClientSpecific();
    virtual ~NodeClientSpecific();
    NodeClientSpecific(ClientSpecificType t);

    std::string exec(SPtr<TSrvMsg> msg);
    static void  analyseMessage(SPtr<TSrvMsg> msg);

    static std::string vendor_spec_num;
    static std::string vendor_spec_data;
    static std::string vendor_class_num;
    static std::string vendor_class_data;
    static SPtr<TSrvMsg> CurrentMsg;

private:
    ClientSpecificType Type;

};


#endif /* NODECLIENTSPECIFIC_H_ */
