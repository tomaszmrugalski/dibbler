/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 */

#ifndef SPtr_H
#define SPtr_H

#include <iostream>

//Don't use this class alone, it's used only in casting 
//one smartpointer to another smartpointer 
//e.g.
//SPtr<a> a(new a()); SPtr<b> b(new(b)); a=b;

class Ptr {
public:
    //constructor used in case of NULL SPtr
    Ptr() {
	      ptr=NULL;
	      refcount=1;
    }
    //Constructor used in case of non NULL SPtr
    Ptr(void* object) {
	      ptr=object;
	      refcount=1;
    }
    
    ~Ptr() {
      	//if(ptr) delete ptr;
    }
    int refcount; //refrence counter
    void * ptr;	  //pointer to the real object
};

template <class T>
class SPtr
{

public:
    SPtr();
    SPtr(T* something);
	SPtr(Ptr *voidptr) { 
        if(voidptr) 
        {
            this->ptr=voidptr; 
            this->ptr->refcount++;
        }
        else
            this->ptr=new Ptr();
    }
    SPtr(const SPtr & ref);
	SPtr(int onlyNull);
	SPtr& operator=(const SPtr& old);

	operator Ptr*() {
	    if (this->ptr->ptr) 
		return this->ptr;
	    else
		return (Ptr*)NULL;
	}

    int refCount();
    ~SPtr();
    T& operator*() const;
    T* operator->() const;

 private:
    Ptr * ptr;
};

template <class T> SPtr<T>::SPtr() {
    ptr = new Ptr();
}

template <class T> int SPtr<T>::refCount() {
    if (this->ptr)
	return this->ptr->refcount;
    return 0;
}

template <class T>
SPtr<T>::SPtr(T* something) {
    ptr = new Ptr(something);
}

template <class T>
SPtr<T>::SPtr(const SPtr& old) {
	old.ptr->refcount++;
	this->ptr = old.ptr;
    this->ptr->refcount=old.ptr->refcount;
}

template <class T>
SPtr<T>::~SPtr() {
    if (!(--(ptr->refcount))) {
	delete (T*)(ptr->ptr);
	delete ptr;
    }
}

template <class T>
T& SPtr<T>::operator*() const {
    return *((T*)(ptr->ptr)); //it can return NULL
}

template <class T>
T* SPtr<T>::operator->() const {
    if (!ptr) {
	return 0;
    }
    return (T*)(ptr->ptr); //it can return NULL
}

//It's is called in eg. instrusction: return NULL;
//and SPtr is returned in function
template <class T>
SPtr<T>::SPtr(int )
{
	ptr=new Ptr(); //this->ptr->ptr is NULL
}

template <class T>
SPtr<T>& SPtr<T>::operator=(const SPtr& old) {
	if (this==&old)
		return *this;
	if (this->ptr) 
		if(!(--this->ptr->refcount))
		{
		    delete (T*)(this->ptr->ptr);
		    delete this->ptr;
		    this->ptr=NULL;
		}
		this->ptr=old.ptr;
		old.ptr->refcount++;
		//    cout << "operator=" << endl;
		return *this;
}
#endif
