/* 
 * File:   CURLWrap.h
 * Author: Michal Golon
 * 
 */

#ifndef CURLWRAP_H
#define	CURLWRAP_H

#include "curl/curl.h"
namespace Wrapper {
    class CURLWrap {
    public:
        CURLWrap();
        ~CURLWrap();

        void cleanup();
        void perform();
        bool isOK();
        CURLcode getErrorCode();

        template <typename T, typename V>
        void setOption(T option, V parameter)
        {
            curl_easy_setopt(curl, option, parameter);
        }

    private:
        CURL* curl;
        CURLcode code;
    };
}

#endif	/* CURLWRAP_H */

