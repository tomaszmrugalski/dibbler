#ifndef OPT_H
#define OPT_H

#include "SmartPtr.h"
#include "Container.h"

class TMsg;

class TOpt
{
  public:
    TOpt(int optType, TMsg* parent);
    virtual ~TOpt();
    
    virtual int getSize() = 0;

    virtual char * storeSelf(char* buf) = 0;

    virtual bool doDuties() = 0;
	
    //If option should be checked for containnig consistent values these
    //method should rewritten e.g. in OPTIA_NA T1<T2, in IAAddress valid>pref
    //but there is no too many options, in which it is required
    virtual bool isValid();

    int getOptType();
    
    int getSubOptSize();
    
    char* TOpt::storeSubOpt(char* buf);
    SmartPtr<TOpt> getOption(int optType);

    // suboptions management
    void firstOption();
    SmartPtr<TOpt> getOption();
    void addOption(SmartPtr<TOpt> opt);
    int countOption();
    void setParent(TMsg* Parent);
    

 protected:
    TContainer< SmartPtr<TOpt> > SubOptions;
    int OptType;
    TMsg* Parent;
};

#endif
