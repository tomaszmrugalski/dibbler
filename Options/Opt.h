/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: Opt.h,v 1.6 2006-01-12 00:23:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2004/10/27 22:07:56  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.4  2004/05/23 16:40:12  thomson
 * *** empty log message ***
 *
 *                                                                           
 */

#ifndef OPT_H
#define OPT_H

#include "SmartPtr.h"
#include "Container.h"
#include "DUID.h"

class TMsg;

class TOpt
{
  public:
    TOpt(int optType, TMsg* parent);
    virtual ~TOpt();

    virtual int getSize() = 0;
    virtual char * storeSelf(char* buf) = 0;
    virtual bool doDuties() = 0;
    virtual bool isValid();
    int getOptType();
    int getSubOptSize();
    
    char* storeSubOpt(char* buf);
    SmartPtr<TOpt> getOption(int optType);

    // suboptions management
    void firstOption();
    SmartPtr<TOpt> getOption();
    void addOption(SmartPtr<TOpt> opt);
    int countOption();
    void setParent(TMsg* Parent);

    SmartPtr<TDUID> getDUID();
    void setDUID(SmartPtr<TDUID> duid);

 protected:
    TContainer< SmartPtr<TOpt> > SubOptions;
    int OptType;
    TMsg* Parent;
    SmartPtr<TDUID> DUID;
};

#endif
