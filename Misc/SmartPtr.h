/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: SmartPtr.h,v 1.2 2004-03-29 22:06:49 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 * Released under GNU GPL v2 licence
 *
 */

#ifndef SMARTPTR_H
#define SMARTPTR_H

#include <stdio.h>

class Ptr {
public:
	//constructor used in case of NULL SmartPtr
	Ptr() {
		ptr=NULL;
		refcount=1;
	}
	//Constructor used in case of non NULL SmartPtr
	Ptr(void* sth) {
	    ptr=sth;
	    refcount=1;
	}

	~Ptr() {
	    //if(ptr) delete ptr;
	}
	int refcount; //refrence counter
	void * ptr;	  //pointer to the real object
};

template <class T>
class SmartPtr
{

	//Don't use this class alone, it's used only in casting 
	//one smartpointer to another smartpointer 
	//e.g.
	//SmartPtr<a> a(new a()); SmartPtr<b> b(new(b)); a=b;

public:
    SmartPtr();
    SmartPtr(T* something);
	SmartPtr(Ptr *voidptr) { 
        if(voidptr) 
        {
            this->ptr=voidptr; 
            this->ptr->refcount++;
        }
        else
            this->ptr=new Ptr();
    }
    SmartPtr(const SmartPtr & ref);
	SmartPtr(int onlyNull);
	SmartPtr& operator=(const SmartPtr& old);

	operator Ptr*() {if (this->ptr->ptr) 
						return this->ptr;
					else
						return (Ptr*)NULL;
					}
	//operator int() {return ptr->ptr?1:NULL;};

    //bool operator==(SmartPtr& right);
    //SmartPtr& operator=(const SmartPtr& old);
    ~SmartPtr();
    T& operator*() const;
    T* operator->() const;

 private:
    Ptr * ptr;
};

template <class T>

SmartPtr<T>::SmartPtr() {
    ptr = new Ptr();
}

template <class T>
SmartPtr<T>::SmartPtr(T* something) {
    ptr = new Ptr(something);
}

template <class T>
SmartPtr<T>::SmartPtr(const SmartPtr& old) {
	old.ptr->refcount++;
	this->ptr = old.ptr;
    this->ptr->refcount=old.ptr->refcount;
}

template <class T>
SmartPtr<T>::~SmartPtr() {
    if (!(--(ptr->refcount))) {
	delete (T*)(ptr->ptr);
	delete ptr;
    }
}

template <class T>
T& SmartPtr<T>::operator*() const {
    return *((T*)(ptr->ptr)); //it can return NULL
}

template <class T>
T* SmartPtr<T>::operator->() const {
    return (T*)(ptr->ptr); //it can return NULL
}

//It's is called in eg. instrusction: return NULL;
//and SmartPtr is returned in function
template <class T>
SmartPtr<T>::SmartPtr(int )
{
	ptr=new Ptr(); //this->ptr->ptr is NULL
}

template <class T>
SmartPtr<T>& SmartPtr<T>::operator=(const SmartPtr& old) {
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
