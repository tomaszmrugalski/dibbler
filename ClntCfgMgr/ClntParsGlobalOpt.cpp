/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntParsGlobalOpt.cpp,v 1.6 2006-08-22 00:01:20 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2004-11-29 21:21:56  thomson
 * Client parser now supports 'option lifetime' directive (bug #75)
 *
 * Revision 1.4  2004/10/25 20:45:52  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.3  2004/05/23 22:37:54  thomson
 * *** empty log message ***
 *
 *                                                                           
 */

#include "ClntParsGlobalOpt.h"
#include "Portable.h"
TClntParsGlobalOpt::TClntParsGlobalOpt() 
    :TClntParsIfaceOpt() {
    this->WorkDir  = WORKDIR;
    this->PrefixLength = CLIENT_DEFAULT_PREFIX_LENGTH;
}

TClntParsGlobalOpt::~TClntParsGlobalOpt() {
}


TClntParsGlobalOpt::TClntParsGlobalOpt(TClntParsGlobalOpt *old) {
    this->WorkDir = old->WorkDir;
}

void TClntParsGlobalOpt::setWorkDir(string dir) {
    this->WorkDir=dir;
}

string TClntParsGlobalOpt::getWorkDir() {
    return this->WorkDir;
}

void TClntParsGlobalOpt::setPrefixLength(int len) {
    this->PrefixLength = len;
}

int TClntParsGlobalOpt::getPrefixLength() {
    return this->PrefixLength;
}
