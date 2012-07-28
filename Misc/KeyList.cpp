/*
 * Dibbler - a portable DHCPv6
 *
 * author : Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: KeyList.cpp,v 1.3 2008-08-29 00:07:30 thomson Exp $
 */

#include "KeyList.h"
#include "Logger.h"

KeyList::~KeyList() {
        KeyListElement * tmp1 = beginning, * tmp2;
        while(tmp1) {
                tmp2 = tmp1;
                tmp1 = tmp1->next;
                delete tmp2->AuthInfoKey;
                delete tmp2;
        }
}

#include <stdio.h>
void KeyList::Add(uint32_t SPI, uint32_t AAASPI, char * AuthInfoKey) {
    char buf[100];
    sprintf(buf, "Auth: Key (SPI: 0x%8.8x, AAASPI: 0x%8.8x, pointer: %p) added to keylist.", SPI, AAASPI, AuthInfoKey);
    Log(Debug) << buf << LogEnd;

    if (AuthInfoKey == NULL) {
        Log(Error) << "Auth: AuthInfoKey is NULL. Probably internal error." << LogEnd;
        return;
    }

    KeyListElement * act = beginning;
    KeyListElement * act2 = NULL;
    while (act) {
        if (act->SPI == SPI) {
            Log(Debug) << "Auth: Strange, SPI already exists in KeyList" << LogEnd;
            return;
        }
        act2 = act;
        act = act->next;
    }

    KeyListElement * new_el = new KeyListElement;
    new_el->SPI = SPI;
    new_el->AAASPI = AAASPI;
    new_el->AuthInfoKey = new char[AUTHKEYLEN];
    memcpy(new_el->AuthInfoKey, AuthInfoKey, AUTHKEYLEN);
    new_el->next = NULL;
    if (act2)
        act2->next = new_el;
    else
        beginning = new_el;
}

char * KeyList::Get(uint32_t SPI) {
    // char buf[100];
    //sprintf(buf, "KeyList get: 0x%8.8x", SPI);
    //Log(Debug) << buf << LogEnd;

    KeyListElement * tmp = beginning;
    while (tmp) {
        if (tmp->SPI == SPI)
            return tmp->AuthInfoKey;
        tmp = tmp->next;
    }

    Log(Warning) << "Auth: Required key (SPI=" << SPI << ") not found." << LogEnd;
    return NULL;
}

void KeyList::Del(uint32_t SPI) {
        KeyListElement * tmp = beginning, * prev = NULL;
        while (tmp) {
                if (tmp->SPI == SPI) {
                        if (prev)
                                prev->next = tmp->next;
                        else
                                beginning = tmp->next;

                        delete tmp->AuthInfoKey;
                        delete tmp;
                        return;
                }
                prev = tmp;
                tmp = tmp->next;
        }
}
