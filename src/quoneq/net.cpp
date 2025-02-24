/*
 * This file is part of the Quoneq library.
 * Copyright (c) 2025 Nathanne Isip
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <quoneq/net.hpp>

#include <curl/curl.h>

std::string quoneq_net::cacert_path = "";

void quoneq_net::init() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void quoneq_net::cleanup() {
    curl_global_cleanup();
}

void quoneq_net::set_ca_cert(std::string path) {
    quoneq_net::cacert_path = path;
}

std::string quoneq_net::get_ca_cert() {
    char* cacert = nullptr;
    CURL *curl = curl_easy_init();

    if(!curl)
        return quoneq_net::cacert_path;

    curl_easy_getinfo(curl, CURLINFO_CAINFO, &cacert);
    if((cacert != NULL) && (cacert[0] == '\0'))
        return std::string(cacert);

    return quoneq_net::cacert_path; 
}
