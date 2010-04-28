/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: ClntParsGlobalOpt.cpp,v 1.20 2008-08-30 21:41:11 thomson Exp $
 *
 */

#include "ClntParsGlobalOpt.h"
#include "Portable.h"
#include "DHCPConst.h"
#include "Logger.h"

TClntParsGlobalOpt::TClntParsGlobalOpt() 
    :TClntParsIfaceOpt() {
    this->WorkDir        = WORKDIR;
    this->PrefixLength   = CLIENT_DEFAULT_PREFIX_LENGTH;
    this->Digest         = CLIENT_DEFAULT_DIGEST;
    this->ScriptsDir     = DEFAULT_SCRIPTSDIR;
    this->NotifyScripts  = false;
    this->AnonInfRequest = false;
    this->InactiveMode   = false;
    this->InsistMode     = false;
    this->FQDNFlagS      = false;
    this->Experimental   = false;
    this->ExperimentalMappingPrefix = false;
    this->UseConfirm     = true;

    this->AuthEnabled    = false;
    this->AuthAcceptMethods.clear();
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

void TClntParsGlobalOpt::setInsistMode(bool insist)
{
    InsistMode = insist;
}

bool TClntParsGlobalOpt::getInsistMode()
{
    return InsistMode;
}

void TClntParsGlobalOpt::setInactiveMode(bool flex)
{
    InactiveMode = flex;
}

bool TClntParsGlobalOpt::getInactiveMode()
{
    return InactiveMode;
}

void TClntParsGlobalOpt::setExperimental()
{
    Experimental = true;
}

bool TClntParsGlobalOpt::getExperimental()
{
    return Experimental;
}

void TClntParsGlobalOpt::setAuthAcceptMethods(List(DigestTypes) lst)
{
    AuthAcceptMethods = lst;
    Log(Debug) << "AUTH: " << lst.count() << " method(s) accepted." << LogEnd;
}

List(DigestTypes) TClntParsGlobalOpt::getAuthAcceptMethods()
{
    return AuthAcceptMethods;
}

void TClntParsGlobalOpt::setFQDNFlagS(bool s)
{
    FQDNFlagS = s;
}

bool TClntParsGlobalOpt::getFQDNFlagS()
{
    return FQDNFlagS;
}

void TClntParsGlobalOpt::setAuthEnabled(bool enabled)
{
    AuthEnabled = enabled;
    Log(Debug) << "AUTH: Authentication " << (enabled?"enabled":"disabled") << "." << LogEnd;
}

bool TClntParsGlobalOpt::getAuthEnabled()
{
    return AuthEnabled;
}

void TClntParsGlobalOpt::setMappingPrefix(bool useMP)
{
    ExperimentalMappingPrefix = useMP;
    Log(Notice) << "Experimental: mappix-prefix " << (useMP?"enabled":"disabled") << LogEnd;
}

bool TClntParsGlobalOpt::getMappingPrefix()
{
    return ExperimentalMappingPrefix;
}

void TClntParsGlobalOpt::setConfirm(bool conf)
{
    UseConfirm = conf;
}

bool TClntParsGlobalOpt::getConfirm()
{
    return UseConfirm;
}

bool TClntParsGlobalOpt::getNotifyScripts()
{
    return NotifyScripts;
}

void TClntParsGlobalOpt::setNotifyScripts(bool useScripts)
{
    NotifyScripts = useScripts;
}
