/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelParsGlobalOpt.h,v 1.1 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef TRELPARSGLOBALOPT_H_
#define TRELPARSGLOBALOPT_H_
#include "RelParsIfaceOpt.h"

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
