/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntParsGlobalOpt.cpp,v 1.7 2006-11-17 00:39:55 thomson Exp $
 *
 */

#include "ClntParsGlobalOpt.h"
#include "Portable.h"
#include "DHCPConst.h"

TClntParsGlobalOpt::TClntParsGlobalOpt() 
    :TClntParsIfaceOpt() {
    this->WorkDir      = WORKDIR;
    this->PrefixLength = CLIENT_DEFAULT_PREFIX_LENGTH;
    this->Digest       = CLIENT_DEFAULT_DIGEST;
}

TClntParsGlobalOpt::~TClntParsGlobalOpt() {
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

void TClntParsGlobalOpt::setDigest(DigestTypes digest) {
    this->Digest = digest;
}

DigestTypes TClntParsGlobalOpt::getDigest() {
    return this->Digest;
}

