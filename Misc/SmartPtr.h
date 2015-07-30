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
        ptr = NULL;
        refcount = 1;
    }
    //Constructor used in case of non NULL SPtr
    Ptr(void* object) {
        ptr = object;
        refcount = 1;
    }

    ~Ptr() {
    }

    int refcount; //refrence counter
    void * ptr;   //pointer to the real object
};

template <class T>
class SPtr
{

public:
    SPtr();
    SPtr(T* something);

    SPtr(Ptr* voidptr) {
        if(voidptr) {
            ptr = voidptr;
            ptr->refcount++;
        } else {
            ptr = new Ptr();
        }
    }

    SPtr(const SPtr& ref);

    SPtr& operator=(const SPtr& old);

    /// @brief Resets a pointer (essentially assign NULL value)
    void reset() {
        decrease_reference();
        ptr = new Ptr();
    }

    /// @brief re-sets the pointer to point to the new object
    ///
    /// @param obj pointer to the new object (may be NULL)
    void reset(T* obj) {
        decrease_reference();
        ptr = new Ptr(obj);
    }

    /// @brief Returns pointer to the underlying object
    ///
    /// @todo Is this really used?
    ///
    /// @return raw pointer to the object
    operator Ptr*() {
      if (ptr->ptr)
        return ptr;
      else
        return (Ptr*)NULL;
    }

    operator const Ptr*() const {
      if (ptr->ptr)
        return ptr;
      else
        return (Ptr*)NULL;
    }

    int refCount();
    ~SPtr();
    T& operator*() const;
    T* operator->() const;

    const T* get() const;

    /// @brief Attempts to dynamic cast to SmartPtr<to>
    /// @tparam to derived class
    /// @return SmartPtr to the derived class (or NULL if cast failed)
    template<class to>
    SPtr<to> dynamic_pointer_cast() {
        
        // Null pointer => return null pointer, too.
        if (ptr->ptr == NULL) {
            return SPtr<to>();
        }

        // Try to dynamic cast the underlying pointer.
        to* tmp = dynamic_cast<to*>( static_cast<T*>(ptr->ptr) );
        if (tmp) {
            // Cast was successful? Then return SmartPtr of the derived type
            return SPtr<to>(ptr);
        } else {
            // Cast failed? Ok, incorrect type. Return null
            return SPtr<to>();
        }
    }

private:
    void decrease_reference();
    
    Ptr * ptr;
};
    

template <class T>
void SPtr<T>::decrease_reference() {
    if (!(--(ptr->refcount))) {
        if (ptr->ptr) {
            delete (T*)(ptr->ptr);
        }
        delete ptr;
    }
}

template <class T> SPtr<T>::SPtr() {
    ptr = new Ptr();
}

template <class T> int SPtr<T>::refCount() {
    if (ptr) {
        return ptr->refcount;
    }
    return 0;
}

template <class T>
SPtr<T>::SPtr(T* something) {
    ptr = new Ptr(something);
}

template <class T>
SPtr<T>::SPtr(const SPtr& old) {

    // #include <typeinfo>
    // std::cout << "### Copy constr " << typeid(T).name() << std::endl;
    old.ptr->refcount++;
    ptr = old.ptr;

    // This doesn't make sense. It just copies value to itself
    ptr->refcount = old.ptr->refcount;
}

template <class T>
SPtr<T>::~SPtr() {
    decrease_reference();
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

template <class T>
const T* SPtr<T>::get() const {
    if (!ptr) {
        return 0;
    }
    return (T*)(ptr->ptr);
}

template <class T>
SPtr<T>& SPtr<T>::operator=(const SPtr& old) {
    if (this==&old)
        return *this;

    // If this pointer points to something...
    if (this->ptr) {
        if(!(--this->ptr->refcount))
        {
            if (this->ptr->ptr) {
                // delete the object itself
                delete (T*)(this->ptr->ptr);
            }
            // now delete its reference
            delete this->ptr;
            this->ptr = NULL;
        }
    }
    this->ptr=old.ptr;
    old.ptr->refcount++;
    return *this;
}
#endif
