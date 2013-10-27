#include "DUID.h"
#include "SmartPtr.h"

#include <string>
#include <gtest/gtest.h>

using namespace std;

namespace {

TEST(DUIDTest, constructor) {

    char expData[] = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef };
    char buffer[100];

    SPtr<TDUID> duid1 = new TDUID("1234567890abcdef");
    ASSERT_EQ(8u, duid1->getLen());
    EXPECT_TRUE(0 == memcmp(expData, duid1->get(), 8));
    duid1->storeSelf(buffer);
    EXPECT_TRUE(0 == memcmp(buffer, expData, 8));
    EXPECT_EQ("12:34:56:78:90:ab:cd:ef", duid1->getPlain());

    SPtr<TDUID> duid2 = new TDUID("12:34:56:78:90:ab:cd:ef");
    ASSERT_EQ(8u, duid2->getLen());
    EXPECT_TRUE(0 == memcmp(expData, duid2->get(), 8));
    memset(buffer, 0, sizeof(buffer));
    duid2->storeSelf(buffer);
    EXPECT_TRUE(0 == memcmp(buffer, expData, 8));
    EXPECT_EQ("12:34:56:78:90:ab:cd:ef", duid2->getPlain());

    EXPECT_TRUE(*duid1 == *duid2);

    SPtr<TDUID> duid3 = new TDUID(expData, sizeof(expData));
    ASSERT_EQ(8u, duid3->getLen());
    EXPECT_TRUE(0 == memcmp(expData, duid3->get(), 8));
    memset(buffer, 0, sizeof(buffer));
    duid3->storeSelf(buffer);
    EXPECT_TRUE(0 == memcmp(buffer, expData, 8));
    EXPECT_EQ("12:34:56:78:90:ab:cd:ef", duid3->getPlain());

    EXPECT_TRUE(*duid1 == *duid3);
    EXPECT_TRUE(*duid2 == *duid3);
}

TEST(DuidTest, operators) {
    TDUID duid1("123456");
    TDUID duid2("1234"); // shorter
    TDUID duid3("f23456"); // greater
    TDUID duid4("123455"); // smaller
    TDUID duid5("123456");

    EXPECT_TRUE(duid2 <= duid1);
    EXPECT_TRUE(duid1 <= duid3);
    EXPECT_TRUE(duid4 <= duid1);
    EXPECT_TRUE(duid1 == duid5);
}

}
