/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelParsGlobalOpt.cpp,v 1.1 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "RelParsGlobalOpt.h"
#include "Portable.h"

TRelParsGlobalOpt::TRelParsGlobalOpt(void) {
    this->WorkDir  = WORKDIR;
}

TRelParsGlobalOpt::~TRelParsGlobalOpt(void) {
}

string TRelParsGlobalOpt::getWorkDir() {
    return this->WorkDir;
}

void TRelParsGlobalOpt::setWorkDir(string dir) {
    this->WorkDir=dir;
}

