/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: KeyList.h,v 1.1 2008-02-25 17:49:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#include "DHCPConst.h"
#include <stdlib.h>
#include <string.h>

#ifndef  KEYLIST_FILE_HEADER_INC
#define  KEYLIST_FILE_HEADER_INC

struct KeyListElement {
        // client-server SPI
        uint32_t SPI;

        uint32_t AAASPI;
        char * AuthInfoKey;

        KeyListElement * next;
};

class KeyList {
        public:
                KeyList(): beginning(NULL) {}
                ~KeyList();
                void Add(uint32_t SPI, uint32_t AAASPI, char * AuthInfoKey);
                void Del(uint32_t SPI);
                char * Get(uint32_t SPI);
        protected:
                KeyListElement * beginning;
};

#endif   /* ----- #ifndef KEYLIST_FILE_HEADER_INC  ----- */
