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

#include <quoneq/tor.hpp>

std::unique_ptr<quoneq_http_response> quoneq_tor_client::get(
    const std::string& url, 
    const std::map<std::string, std::string>& headers,
    const std::map<std::string, std::string>& cookies,
    const std::string& username,
    const std::string& password
) {
    return quoneq_http_client::get(
        url,
        headers,
        cookies,
        "socks5h://localhost:9050",
        username,
        password
    );
}

std::unique_ptr<quoneq_http_response> quoneq_tor_client::post(
    const std::string& url,
    const std::map<std::string, std::string>& form,
    const std::map<std::string, std::string>& headers,
    const std::map<std::string, std::string>& cookies,
    const std::map<std::string, std::string>& files,
    const std::string& username,
    const std::string& password
) {
    return quoneq_http_client::post(
        url,
        form,
        headers,
        cookies,
        files,
        "socks5h://localhost:9050",
        username,
        password
    );
}

std::unique_ptr<quoneq_http_response> quoneq_tor_client::ping(
    const std::string& url, 
    const std::string& username,
    const std::string& password
) {
    return quoneq_http_client::ping(
        url,
        "socks5h://localhost:9050",
        username,
        password
    );
}

bool quoneq_tor_client::is_tor_running() {
    auto response = quoneq_http_client::ping(
        "https://check.torproject.org",
        "socks5h://localhost:9050"
    );
    return response && response->status == 200;
}

std::unique_ptr<quoneq_http_response> quoneq_tor_client::download_file(
    const std::string& url,
    const std::string out_filename,
    const std::map<std::string, std::string>& form,
    const std::map<std::string, std::string>& headers,
    const std::map<std::string, std::string>& cookies,
    const std::map<std::string, std::string>& files,
    const std::string& username,
    const std::string& password
) {
    return quoneq_http_client::download_file(
        url,
        out_filename,
        form,
        headers,
        cookies,
        files,
        "socks5h://localhost:9050",
        username,
        password
    );
}
