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
        void	first();
        void	delFirst() {
            lista.pop_front();
            first();
        }

        void    del() {
            it--;
            lista.erase(it);
            first();
        }

        void    clear();
        TYP	get() {
            if (it != lista.end()) {
                return *it++;
            } else {
                return TYP();
            }
        }

        TYP getLast() {
            return lista.back();
        }

        TYP getFirst() {
            return lista.front();
        }

        void delLast() {
            lista.pop_back();
            first();
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
