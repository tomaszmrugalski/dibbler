#include "SmartPtr.h"

#include <string>
#include <gtest/gtest.h>

using namespace std;

namespace {

int static_value = 0;

class Base1 {
public:
    Base1()
        :value(1) {
        static_value = value;
    }
    int value;

    virtual ~Base1() {
        static_value = -value;
    }
};

class Derived2 : Base1 {
    Derived2()
        :Base1() {
        value = 2;
        static_value = value;
    }

    virtual ~Derived2() {
        static_value = -value;
    }
};

class Derived3 : Base1 {
    Derived3()
        :Base1() {
        value = 3;
    }

    virtual ~Derived3() {
        static_value = -value;
    }
};

class SPtrTest : public ::testing::Test {
public:
    SPtrTest() {
        static_value = 0;
    }
};

// Checks if NULL pointer is behaving properly
TEST_F(SPtrTest, null) {

    // Check that by default a pointer is NULL
    SPtr<Base1> base;

    EXPECT_FALSE(base);
    EXPECT_FALSE((Ptr*)base);
    EXPECT_EQ(1, base.refCount());
    EXPECT_EQ(0, static_value);
}

// Test checks whether basic pointer operations are working
// properly
TEST_F(SPtrTest, basic_assign) {
    SPtr<Base1> base = new Base1();

    EXPECT_TRUE(base);
    EXPECT_TRUE((Ptr*)base);
    EXPECT_EQ(1, base->value);
    EXPECT_EQ(1, static_value);
    EXPECT_EQ(1, ((Ptr*)base)->refcount);

    // Explicitly assign empty pointer, the object should be deleted
    // and the pointer should be NULL
    base = SPtr<Base1>();
    EXPECT_FALSE(base);
    EXPECT_FALSE((Ptr*)base);
    EXPECT_EQ(-1, static_value);

    // Assign a new object again
    base = new Base1();
    EXPECT_TRUE(base);
    EXPECT_TRUE((Ptr*)base);
    EXPECT_EQ(1, base->value);

    // Explicitly assign NULL, the object should be deleted
    // and the pointer should be NULL
    base = 0;
    EXPECT_FALSE(base);
    EXPECT_FALSE((Ptr*)base);
    EXPECT_EQ(-1, static_value);
}

// This test checks if the reference counting is implemented properly.
TEST_F(SPtrTest, refcounting) {
    SPtr<Base1> a,b,c;

    a = new Base1();
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

    a = 0;
    EXPECT_EQ(1, a.refCount()); // NULL
    EXPECT_EQ(2, b.refCount());
    EXPECT_EQ(2, c.refCount());
    EXPECT_FALSE(a);
    EXPECT_TRUE(b);
    EXPECT_TRUE(c);

    b = 0;
    EXPECT_EQ(1, a.refCount()); // NULL
    EXPECT_EQ(1, b.refCount()); // NULL
    EXPECT_EQ(1, c.refCount());
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    EXPECT_TRUE(c);

    // Check that the object is still not destroyed
    EXPECT_EQ(1, static_value);

    c = 0; // This will destroy the last pointer, thus
           // triggering object destruction
    EXPECT_EQ(-1, static_value);
    EXPECT_FALSE(a);
    EXPECT_FALSE(b);
    EXPECT_FALSE(c);
}

/// @todo Write pointer cast tests

}
