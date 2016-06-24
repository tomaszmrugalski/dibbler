#include <gtest/gtest.h>

#include <OptDUID.h>

using namespace std;

TEST(OptDuidTest, constructors) {

    SPtr<TOptDUID> just_ptr;

    just_ptr.reset(new TOptDUID(OPTION_CLIENTID, SPtr<TDUID>(), NULL));
}
