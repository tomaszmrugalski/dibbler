/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntParsGlobalOpt.h,v 1.4 2004-10-25 20:45:52 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/05/23 22:37:54  thomson
 * *** empty log message ***
 *
 *                                                                           
 */

#ifndef PARSGLOBALOPT_H
#define PARSGLOBALOPT_H

#include "ClntParsIfaceOpt.h"

class TClntParsGlobalOpt : public TClntParsIfaceOpt
{
public:
    TClntParsGlobalOpt();
    TClntParsGlobalOpt(TClntParsGlobalOpt *old);
    ~TClntParsGlobalOpt();

    void setWorkDir(string dir);
    string getWorkDir();

private:
    string WorkDir;
};

#endif
