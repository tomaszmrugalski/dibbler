

#ifndef TEST_POSLIB_UTILS_H
#define TEST_POSLIB_UTILS_H

#include "poslib.h"

namespace test {
    void hexToBin(const std::string& hex, message_buff &dst);
    bool cmpBuffers(const message_buff& a, const message_buff&b);
}

#endif
