#include "SrvOptDomainName.h"

TSrvOptDomainName::TSrvOptDomainName(string domain, TMsg* parent)
    :TOptDomainName(domain, parent)
{
}

TSrvOptDomainName::TSrvOptDomainName(char *buf, int bufsize, TMsg* parent)
    :TOptDomainName(buf,bufsize, parent)
{

}

bool TSrvOptDomainName::doDuties()
{
    return true;
}
