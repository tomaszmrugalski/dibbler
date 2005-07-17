/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptGeneric.h,v 1.2 2005-07-17 19:56:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/12 00:01:06  thomson
 * *** empty log message ***
 *
 *
 */

#ifndef OPTGENERIC_H
#define OPTGENERIC_H

#include "DHCPConst.h"
#include "Opt.h"
#include "DUID.h" 
#include "SmartPtr.h"

class TOptGeneric : public TOpt
{
  public:
    TOptGeneric(int optType, char * data, int dataLen, TMsg* parent);
    TOptGeneric(int optType, TMsg* parent);
    ~TOptGeneric();
    int getSize();
    void setData(char * data, int dataLen);

    char * storeSelf(char* buf);
    bool isValid();
  protected:
    char * Data;
    int DataLen;
};

#endif
