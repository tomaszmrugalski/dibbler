/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <stdlib.h>
#include <string.h>
#include "Portable.h"

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

#endif
