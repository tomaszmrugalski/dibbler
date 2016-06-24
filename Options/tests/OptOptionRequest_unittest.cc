#include <gtest/gtest.h>

#include <OptOptionRequest.h>

using namespace std;

TEST(OptOROTest, constructors) {

    SPtr<TOptOptionRequest> just_ptr;

    SPtr<TOptOptionRequest> oro;
    oro.reset(new TOptOptionRequest(OPTION_ORO, NULL));
}
