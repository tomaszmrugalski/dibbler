/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: NodeOperator.h,v 1.1 2008-10-12 19:36:58 thomson Exp $
 *
 */

#ifndef NODEOPERATOR_H_
#define NODEOPERATOR_H_
#include "Node.h"
#include "SmartPtr.h"
#include <iostream>

class NodeOperator  : public Node {
public:
	enum OperatorType
	{
	    OPERATOR_EQUAL     = 1,
	    OPERATOR_AND       = 2,
	    OPERATOR_OR        = 3,
	    OPERATOR_SUBSTRING = 4,
	    OPERATOR_CONTAIN   = 5
	};

	NodeOperator();
	NodeOperator(OperatorType t , SPtr<Node>& lll, SPtr<Node>& rrr);
	// Construction method for Substring
	NodeOperator(OperatorType t , SPtr<Node>& lll,  int in,  int len);
	// Construction method for Contain
	NodeOperator(OperatorType t, SPtr<Node>& lll,  string s );
	virtual ~NodeOperator();
	virtual string exec(SPtr<TSrvMsg> msg);
	virtual string exec();

private :
	SPtr<Node> l;
	SPtr<Node> r;
	OperatorType type;

	// support substring
	int index;
	int length;

	// support contain
	std::string cotainString;
};

#endif /* NODEOPERATOR_H_ */
