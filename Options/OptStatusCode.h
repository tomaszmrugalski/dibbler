/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: OptStatusCode.h,v 1.2 2004-12-08 00:17:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef OPTSTATUSCODE_H
#define OPTSTATUSCODE_H

#include <string>
#include "Opt.h"
#include "DHCPConst.h"

class TOptStatusCode : public TOpt
{
  public:
    TOptStatusCode( char * &buf,  int &len, TMsg* parent);
    TOptStatusCode(int status,string Message, TMsg* parent);
    
    int getSize();
    char * storeSelf( char* buf);
    
    int getCode();  
    string getText();
    
    bool doDuties();    
 private:
    string Message;
    int StatusCode;
    bool Valid;
};

#endif 
