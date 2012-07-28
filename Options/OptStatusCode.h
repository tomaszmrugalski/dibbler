/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: OptStatusCode.h,v 1.3 2008-08-29 00:07:31 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004-12-08 00:17:04  thomson
 * DOS end of lines removed, header added.
 *
 */

#ifndef OPTSTATUSCODE_H
#define OPTSTATUSCODE_H

#include <string>
#include "Opt.h"
#include "DHCPConst.h"

class TOptStatusCode : public TOpt
{
  public:
    TOptStatusCode(const char * buf, size_t len, TMsg* parent);
    TOptStatusCode(int status, const std::string& Message, TMsg* parent);
    
    size_t getSize();
    char * storeSelf( char* buf);
    
    int getCode();  
    std::string getText();
    
    bool doDuties();    
 private:
    std::string Message_;
    int StatusCode_;
    bool Valid_;
};

#endif 
