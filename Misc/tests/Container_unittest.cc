#include "Container.h"
#include "SmartPtr.h"

#include <string>
#include <gtest/gtest.h>

using namespace std;

namespace {

int static_value = 0;

class Base {
public:
    Base()
        :value(1) {
        static_value++;
    }
    int value;

    virtual ~Base() {
        static_value--;
    }
};


class ContainerTest : public ::testing::Test {
public:
    ContainerTest() {
        static_value = 0;
    }
};

TEST_F(ContainerTest, empty) {
    TContainer< SPtr<Base> > container;

    EXPECT_TRUE(container.empty());
    EXPECT_EQ(0, container.count());

    container.first();
    EXPECT_FALSE(container.get());
    EXPECT_FALSE(container.getFirst());
    EXPECT_FALSE(container.getLast());
}

TEST_F(ContainerTest, singleElement) {
    TContainer< SPtr<Base> > container;

    SPtr<Base> a = new Base();
    SPtr<Base> b = new Base();
    SPtr<Base> c = new Base();

    container.append(a);
    EXPECT_EQ(1, container.count());

    container.first();

    // First call should return the first element
    EXPECT_EQ(a, container.get());

    // Second call should return NULL, as there's no more elements
    EXPECT_FALSE(container.get());

    EXPECT_EQ(a, container.getLast());
    EXPECT_EQ(a, container.getFirst());
}

TEST_F(ContainerTest, 3Elements) {
    TContainer< SPtr<Base> > container;

    SPtr<Base> a = new Base();
    SPtr<Base> b = new Base();
    SPtr<Base> c = new Base();

    container.append(a);
    EXPECT_EQ(1, container.count());

    container.append(b);

    EXPECT_EQ(b, container.getLast());
    EXPECT_EQ(a, container.getFirst());

    container.first();

    // First call should return the first element
    EXPECT_EQ(a, container.get());

    // Second call should get the second element
    EXPECT_EQ(b, container.get());

    // Second call should return NULL, as there's no more elements
    EXPECT_FALSE(container.get());

    container.append(c);
    EXPECT_EQ(a, container.getFirst());
    EXPECT_EQ(c, container.getLast());
}

}
