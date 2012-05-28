/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Nguyen Vinh Nghiem
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
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

        NodeOperator(OperatorType t , SPtr<Node>& lll, SPtr<Node>& rrr);
        // Construction method for Substring
        NodeOperator(OperatorType t , SPtr<Node>& lll,  int in,  int len);
        // Construction method for Contain
        NodeOperator(OperatorType t, SPtr<Node>& lll,  std::string s );
        virtual ~NodeOperator();
        virtual std::string exec(SPtr<TSrvMsg> msg);
        virtual std::string exec();

private :
        OperatorType Type_;
        SPtr<Node> L_;
        SPtr<Node> R_;

        // support substring
        int Index_;
        int Length_;

        // support contain
        std::string ContainString_;
};

#endif /* NODEOPERATOR_H_ */
