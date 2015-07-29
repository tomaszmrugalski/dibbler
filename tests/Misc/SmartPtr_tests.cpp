#include <gtest/gtest.h>
#include "SmartPtr.h"

using namespace std;

namespace test {


int global_cnt;

class foo {
public:
    foo() : value(0) {
        global_cnt++;
    }

    foo(int x) : value(x) {
        global_cnt++;
    }

    ~foo() {
        global_cnt--;
    }
    int value;
};

class SmartPtrTest : public ::testing::Test {
public:
    SmartPtrTest() {
        global_cnt = 0;
    }
    ~SmartPtrTest() {
        if (global_cnt != 0) {
            ADD_FAILURE() << "global reference counter (global_cnt) "
                "expected to be zero, but is " << global_cnt;
        }
    }

};

// Test checks if basic initializations are working as expected.
TEST_F(SmartPtrTest, basic) {
    SPtr<foo> ptr1;
    ASSERT_FALSE(ptr1);

    SPtr<foo> ptr2 = new foo(7);
    EXPECT_TRUE(ptr2);
    EXPECT_EQ(7, ptr2->value);

    // ptr2.reset();
    // EXPECT_FALSE(ptr2);
}

#if 0
TEST_F(SmartPtrTest, casting) {
    class base {

    };

    class derived1 : public base {

    };

    class derived2 : public base {

    };

    SPtr<base> base = new derived1();
    EXPECT_TRUE(base);

    SPtr<derived1> der1 = dynamic_cast<derived1>(base);
    ASSERT_TRUE(der1);

    SPtr<derived2> der2 = dynamic_cast<derived2>(base);
    EXPECT_FALSE(der2);
}
#endif

}


