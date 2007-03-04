/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntParsGlobalOpt.cpp,v 1.9 2007-03-04 22:34:05 thomson Exp $
 *
 */

#include "ClntParsGlobalOpt.h"
#include "Portable.h"
#include "DHCPConst.h"

TClntParsGlobalOpt::TClntParsGlobalOpt() 
    :TClntParsIfaceOpt() {
    this->WorkDir        = WORKDIR;
    this->PrefixLength   = CLIENT_DEFAULT_PREFIX_LENGTH;
    this->Digest         = CLIENT_DEFAULT_DIGEST;
    this->ScriptsDir     = DEFAULT_SCRIPTSDIR;
    this->AnonInfRequest = false;
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

void TClntParsGlobalOpt::setScriptsDir(string dir) {
    this->ScriptsDir=dir;
}

string TClntParsGlobalOpt::getScriptsDir() {
    return this->ScriptsDir;
}

void TClntParsGlobalOpt::setAnonInfRequest(bool anonymous) {
    this->AnonInfRequest = anonymous;
}

bool TClntParsGlobalOpt::getAnonInfRequest() {
    return this->AnonInfRequest;
}
