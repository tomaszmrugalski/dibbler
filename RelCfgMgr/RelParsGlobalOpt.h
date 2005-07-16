/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelParsGlobalOpt.h,v 1.2 2005-07-16 14:46:32 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 */

#ifndef TRELPARSGLOBALOPT_H_
#define TRELPARSGLOBALOPT_H_
#include "RelParsIfaceOpt.h"
#include <string>

class TRelParsGlobalOpt : public TRelParsIfaceOpt
{
 public:
    TRelParsGlobalOpt(void);
    ~TRelParsGlobalOpt(void);
    
    string getWorkDir();
    void setWorkDir(string dir);
    
private:
    string WorkDir;
};
#endif
