#include <string.h>
#include "Portable.h"
#include "OptRemoteId.h"
#include "DHCPConst.h"
#include "Logger.h"


TOptRemoteID::TOptRemoteID(int type, char * buf,  int n, TMsg* parent)
    :TOpt(type, parent){

}

TOptRemoteID::TOptRemoteID(int type, int enterprise, char *data, int dataLen, TMsg* parent)
    :TOpt(type, parent){


}

size_t TOptRemoteID::getSize(){

    SPtr<TOpt> opt;
    unsigned int len = 8; // normal header(4) + remoteId(4)
    firstOption();
    while (opt = getOption()) {
        len += opt->getSize();
    }
    if (len < 5 ) {
        Log(Debug) << "Option-len filed in remoteId option is to short." <<LogEnd;
    }
    return len;

}

char * TOptRemoteID::storeSelf( char* buf) {

    // option-code OPTION_VENDOR_OPTS (2 bytes long)
    buf = writeUint16(buf,OptType);

    // option-len size of total option-data
    buf = writeUint16(buf, getSize()-4);

    // enterprise-number (4 bytes long)


    //TODO: conversion of enterprise number should be here:

    int tmp=0;
    buf = writeUint32(buf,tmp);

    SPtr<TOpt> opt;
    firstOption();

    while (opt = getOption())
    {
        buf = opt->storeSelf(buf);
    }
    buf = storeSubOpt(buf);
    return buf;


}

bool TOptRemoteID::isValid() {
    return 0;
}

unsigned int TOptRemoteID::getRemoteId(){

    return this->RemoteId;
}

std::string TOptRemoteID::getPlain()
{
    return this->Plain;
}

TOptRemoteID::~TOptRemoteID()
{

}

bool TOptRemoteID::doDuties() { return true; }
