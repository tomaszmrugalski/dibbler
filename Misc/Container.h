#ifndef CONTAINER_H
#define CONTAINER_H

#include <list>

using namespace std;

template <class TYP>
class TContainer{
public:
	TContainer();
	~TContainer();

	bool	append(const TYP &foo);
	bool	prepend(const TYP& foo);    
	int	count();
	void	first();
	void	delFirst();
	void    del();
	void    clear();
	TYP	get();
	TYP	getLast();
	TYP	getFirst();
	void	delLast();

private:
	list<TYP> lista;
	list<TYP>::iterator it;
};

template <class TYP>
TContainer<TYP>::TContainer() {
}

template <class TYP>
TContainer<TYP>::~TContainer() {
	lista.clear();
}

template <class TYP>
void TContainer<TYP>::clear() {
	lista.clear();
}

template <class TYP>
int TContainer<TYP>::count() {
	return lista.size();
}

template <class TYP>
bool TContainer<TYP>::append(const TYP& foo) {
	lista.push_back(foo);
	return true;
}
template <class TYP>bool TContainer<TYP>::prepend(const TYP& foo) {
	lista.push_front(foo);    
	return true;
}
template <class TYP>
void TContainer<TYP>::first() {
	it=lista.begin();
	return;
}

template <class TYP>
TYP TContainer<TYP>::get() {
	if (it!=lista.end())	
		return *it++;	
	else		
		return TYP();
}

template <class TYP>
void TContainer<TYP>::delFirst() {
	lista.pop_front();
	first();
}

template <class TYP>
void TContainer<TYP>::del() {
	it--;
	lista.erase(it);
	first();
}

template <class TYP>TYP TContainer<TYP>::getLast() {	
    return lista.back();
}

template <class TYP>void TContainer<TYP>::delLast() {    
    lista.pop_back();    first();
}

template <class TYP>TYP TContainer<TYP>::getFirst() {	
	return lista.front();
}
#endif
