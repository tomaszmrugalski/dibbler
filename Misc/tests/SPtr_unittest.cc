#include "SmartPtr.h"

#include <string>
#include <gtest/gtest.h>

using namespace std;

namespace {

/// @brief Shows creation and destruction of Base,DerivedA,DerivedB objects
///
/// There are 3 classes here:
///          Base
///           |
///    +------+-----+
///    |            |
/// DerivedA     DerivedB
///
/// Each instance of a class increase static_value by a specific
/// value: 1 for Base, 10 for DerivedA and 100 for DerivedB in
/// their constructors. They decrease the value in their destructors.
/// This way the tests can check when a given object is created
/// or destroyed.
int static_value = 0;

class Base {
public:
    Base()
        :value(1) {
        static_value += value;
    }
    int value;

    virtual ~Base() {
        static_value -= value;
    }
};

class DerivedA : public Base {
public:
    DerivedA()
        :Base() {
        value = 10;
        static_value += value;
    }

    virtual ~DerivedA() {
        static_value -= value;
    }
};

class DerivedB : public Base {
public:
    DerivedB()
        :Base() {
        value = 100;
        static_value += value;
    }

    virtual ~DerivedB() {
        static_value -= value;
    }
};

class SmartPtrTest : public ::testing::Test {
public:
    SmartPtrTest() {
        static_value = 0;
    }
};

// Checks if NULL pointer is behaving properly
TEST_F(SmartPtrTest, null) {

    // Check that by default a pointer is NULL
    SPtr<Base> base;

    EXPECT_FALSE(base);
    EXPECT_FALSE((Ptr*)base);
    EXPECT_EQ(1, base.refCount());
    EXPECT_EQ(0, static_value);
}

// Test checks whether basic pointer operations are working
// properly
TEST_F(SmartPtrTest, basic_assign) {
    SPtr<Base> base = new Base();

    EXPECT_TRUE(base);
    EXPECT_TRUE((Ptr*)base);
    EXPECT_EQ(1, base->value);
    EXPECT_EQ(1, static_value);
    EXPECT_EQ(1, ((Ptr*)base)->refcount);

    // Explicitly assign empty pointer, the object should be deleted
    // and the pointer should be NULL
    base = SPtr<Base>();
    EXPECT_FALSE(base);
    EXPECT_FALSE((Ptr*)base);
    EXPECT_EQ(0, static_value);

    // Assign a new object again
    base = new Base();
    EXPECT_TRUE(base);
    EXPECT_TRUE((Ptr*)base);
    EXPECT_EQ(1, base->value);
    EXPECT_EQ(1, static_value);

    // Explicitly assign NULL, the object should be deleted
    // and the pointer should be NULL
    base.reset();
    EXPECT_FALSE(base);
    EXPECT_FALSE((Ptr*)base);
    EXPECT_EQ(0, static_value);
}

// This test checks if the reference counting is implemented properly.
TEST_F(SmartPtrTest, refcounting) {
    SPtr<Base> a,b,c;

    a = new Base();
    EXPECT_TRUE(a);
    EXPECT_FALSE(b);
    EXPECT_FALSE(c);

    EXPECT_EQ(1, a.refCount());
    b = a;
    EXPECT_EQ(2, a.refCount());
    EXPECT_EQ(2, b.refCount());
    EXPECT_TRUE(a);
    EXPECT_TRUE(b);
    EXPECT_FALSE(c);

    c = b;
    EXPECT_EQ(3, a.refCount()); // non-NULL
    EXPECT_EQ(3, b.refCount()); // non-NULL
    EXPECT_EQ(3, c.refCount()); // non-NULL
    EXPECT_TRUE(a);
    EXPECT_TRUE(b);
    EXPECT_TRUE(c);

    a.reset();
    EXPECT_EQ(1, a.refCount()); // NULL
    EXPECT_EQ(2, b.refCount());
    EXPECT_EQ(2, c.refCount());
    EXPECT_FALSE(a);
    EXPECT_TRUE(b);
    EXPECT_TRUE(c);

    b.reset();
    EXPECT_EQ(1, a.refCount()); // NULL
    EXPECT_EQ(1, b.refCount()); // NULL
    EXPECT_EQ(1, c.refCount());
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    EXPECT_TRUE(c);

    // Check that the object is still not destroyed
    EXPECT_EQ(1, static_value);

    c.reset(); // This will destroy the last pointer, thus
               // triggering object destruction
    EXPECT_EQ(0, static_value);
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    EXPECT_FALSE(c);
}

// This test checks whether a pointer can be successfully cast to derived
// class pointer.
TEST_F(SmartPtrTest, pointer_cast_success) {
    // Create an instance of a derived class and keep base pointer to it.
    SPtr<Base> base(new DerivedA());
    EXPECT_EQ(1, base.refCount());

    // Try to cast it to derived pointer
    SPtr<DerivedA> derived = base.dynamic_pointer_cast<DerivedA>();
    ASSERT_TRUE(derived);
    EXPECT_EQ(2, base.refCount());
    EXPECT_EQ(2, derived.refCount());

    // Case 1 after casting: reset the derived pointer.
    // The reference counter should be decreased back to 1.
    derived.reset();
    ASSERT_EQ(1, base.refCount());

    // Cast 2 after casting: reset the base pointer.
    // The reference counter should be decreased back to 1 as well.
    derived = base.dynamic_pointer_cast<DerivedA>();
    base.reset();
    ASSERT_EQ(1, derived.refCount());
}

// This test checks if the reference counting is implemented properly.
TEST_F(SmartPtrTest, pointer_cast_failure) {

    // We have a single object.
    SPtr<Base> base = new Base();
    ASSERT_EQ(1, base.refCount());

    // Let's try to cast it to derived pointer. It should fail as the
    // object is of type Base, not DerivedA.
    SPtr<DerivedA> derived = base.dynamic_pointer_cast<DerivedA>();
    ASSERT_FALSE(derived);
    ASSERT_EQ(1, base.refCount());

    // Attempt to cast to DerivedB should fail, too.
    ASSERT_FALSE(base.dynamic_pointer_cast<DerivedB>());
    ASSERT_EQ(1, base.refCount());
}

} // end of anonymous namespace
