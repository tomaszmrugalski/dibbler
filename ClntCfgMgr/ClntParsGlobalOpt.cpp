/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntParsGlobalOpt.cpp,v 1.4 2004-10-25 20:45:52 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/05/23 22:37:54  thomson
 * *** empty log message ***
 *
 *                                                                           
 */

#include "ClntParsGlobalOpt.h"
#include "Portable.h"
TClntParsGlobalOpt::TClntParsGlobalOpt() 
    :TClntParsIfaceOpt() {
    WorkDir  = WORKDIR;
}

TClntParsGlobalOpt::~TClntParsGlobalOpt() {
}


TClntParsGlobalOpt::TClntParsGlobalOpt(TClntParsGlobalOpt *old) {
}


void TClntParsGlobalOpt::setWorkDir(string dir) {
    this->WorkDir=dir;
}

string TClntParsGlobalOpt::getWorkDir() {
    return this->WorkDir;
}
