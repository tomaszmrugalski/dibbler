/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: Opt.h,v 1.12 2009-03-24 23:17:18 thomson Exp $
 *
 */

#ifndef OPT_H
#define OPT_H

#include <stdint.h>
#include <list>
#include "SmartPtr.h"
#include "Container.h"
#include "DUID.h"

class TMsg;

class TOpt
{
  public:

    /* this is required to specify, what is the format of expected options.
       This cannot be class field or method, because there is no object
       to hold that information. Option object is created when requested
       option is received. */
    typedef enum {
	Layout_Addr,
	Layout_AddrLst,
	Layout_String,
	Layout_StringLst,
	Layout_Duid,
	Layout_Generic
    } EOptionLayout;

    TOpt(int optType, TMsg* parent);
    virtual ~TOpt();

    /**
     * Return the size of the option, including :
     *  - Option number,
     *  - Option size
     *  - data
     *
     * @return the size
     */
    virtual size_t getSize() = 0;

    /**
     * This method transform the instance of the option class into bytecode
     * ready to be sent to the client or server, conform to the RFC
     *
     * @param buf The address where to store the option
     * @return The address where the option ends
     */
    virtual char * storeSelf(char* buf) = 0;
    virtual bool doDuties() = 0;

    /**
     * Validate the option
     *
     * @return true if the option is valid.
     */
    virtual bool isValid();

    virtual std::string getPlain();

    int getOptType();

    SPtr<TOpt> getOption(int optType);

    // suboptions management
    void firstOption();
    SPtr<TOpt> getOption();
    void addOption(SPtr<TOpt> opt);
    void delOption();
    int countOption();
    void delAllOptions();
    void setParent(TMsg* Parent);

    SPtr<TDUID> getDUID();
    void setDUID(SPtr<TDUID> duid);

    static bool parseOptions(TContainer< SPtr<TOpt> >& options,
                             const char* buf,
                             size_t len,
                             TMsg* parent,
                             uint16_t placeId = 0, // 5 (option 5) or (message 5)
                             std::string place = "option" // "option" or "message"
                             );

 protected:
    char* storeHeader(char* buf);
    char* storeSubOpt(char* buf);
    int getSubOptSize();

    TContainer< SPtr<TOpt> > SubOptions;
    int OptType;
    TMsg* Parent;
    SPtr<TDUID> DUID;
    bool Valid;
};

typedef std::list< SPtr<TOpt> > TOptList;

#endif
