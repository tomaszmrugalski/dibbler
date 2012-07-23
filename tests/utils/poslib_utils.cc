#include <iostream>
#include <gtest/gtest.h>
#include "poslib.h"

using namespace std;

namespace test {

    /// @brief convert hex string (from wireshark export) to binary format
    ///
    /// @param hex
    /// @param dst
    void hexToBin(const std::string& hex, message_buff &dst) {
        size_t len = hex.length()/2;
        // keep this C-style, as message_buff desctructor does free()
        unsigned char * bin = (unsigned char*)malloc(len);

        for (unsigned int i = 0; i < 2*len; i+=2) {
            if (!isxdigit(hex[i]) || !isxdigit(hex[i+1])) {
                throw ("Invalid character ");
            }
            bin[i/2] = (hex[i]-'0')*(isdigit(hex[i])>0)
                + (tolower(hex[i])-'a' + 10)*(isalpha(hex[i])>0);
            bin[i/2] <<= 4;
            bin[i/2] += (hex[i+1]-'0')*(isdigit(hex[i+1])>0)
                + (tolower(hex[i+1])-'a' + 10)*(isalpha(hex[i+1])>0);
        }

        dst.msg = bin;
        dst.len = len;
        dst.is_static = false;
    }

    /// @brief compares two buffers
    ///
    /// @param a
    /// @param b
    ///
    /// @return true if buffers contain the same data, false otherwise
    bool cmpBuffers(const message_buff& a, const message_buff&b) {
        if (a.len != b.len)
            return false;

        if (!memcmp(a.msg, b.msg, a.len))
            return true;

        // ok, it differs. Let's compare them

        cout << hex;
        for (int i = 0; i < a.len; i++) {
            if (a.msg[i] != b.msg[i]) {
                cout << "Offset 0x" << i << " differs: " << int(a.msg[i]) 
                     << " vs " << int(b.msg[i]) << endl;
            }
            
        }
        cout << dec;

        return false;
    }
}
