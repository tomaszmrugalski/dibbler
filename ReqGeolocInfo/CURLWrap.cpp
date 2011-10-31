/* 
 * File:   CURLWrap.cpp
 * Author: Michal Golon
 * 
 */

#include "CURLWrap.h"

Wrapper::CURLWrap::CURLWrap() :
        curl(curl_easy_init()), code(CURLE_OK) {
}

void Wrapper::CURLWrap::cleanup() {
    curl_easy_cleanup(curl);
}

void Wrapper::CURLWrap::perform() {
    code = curl_easy_perform(curl);
}

bool Wrapper::CURLWrap::isOK() {
    return code == CURLE_OK;
}

Wrapper::CURLWrap::~CURLWrap() {
}

CURLcode Wrapper::CURLWrap::getErrorCode() {
    return code;
}

