/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 */

#ifndef CONTAINER_H
#define CONTAINER_H

#include <list>
#include <stdlib.h>

#define List(x) TContainer< SPtr< x > >

template <class TYP>
class TContainer{
public:
    TContainer();
    ~TContainer() {
        lista.clear();
    }

    void append(const TYP &foo) {
        lista.push_back(foo);
    }

    size_t count() const {
        return lista.size();
    }

    bool empty() const {
        return lista.empty();
    }

    void first();

    void delFirst() {
        lista.pop_front();
        first();
    }

    void del() {
        it--;
        lista.erase(it);
        first();
    }

    void clear();

    TYP	get() {
        if (it != lista.end()) {
            return *it++;
        } else {
            return TYP();
        }
    }

    TYP getLast() {
        if (lista.empty()) {
            return TYP();
        } else {
            return lista.back();
        }
    }

    TYP getFirst() {
        if (lista.empty()) {
            return TYP();
        } else {
            return lista.front();
        }
    }

    void delLast() {
        lista.pop_back();
        first();
    }

    /// @brief returns underlying STL container
    ///
    /// @return const reference to the STL container
    const std::list<TYP>& getSTL() const {
        return (lista);
    }

    std::list<TYP>& getSTL() {
        return (lista);
    }
private:
    std::list<TYP> lista;
    typename std::list<TYP>::iterator it;
};

template <class TYP>
TContainer<TYP>::TContainer() {
}

template <class TYP>
void TContainer<TYP>::clear() {
        lista.clear();
}

template <class TYP>
void TContainer<TYP>::first() {
        it=lista.begin();
        return;
}

#endif
