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

#include <quoneq/http.hpp>

#include <chrono>
#include <fstream>
#include <sstream>

size_t quoneq_http_client::write_callback(
    void* contents,
    size_t size,
    size_t nmemb,
    std::string* output
) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

size_t quoneq_http_client::write_file_callback(
    void* contents,
    size_t size,
    size_t nmemb,
    std::ofstream* file
) {
    size_t total_size = size * nmemb;
    file->write(
        (char*)contents,
        static_cast<std::streamsize>(total_size)
    );

    return total_size;
}

size_t quoneq_http_client::header_callback(
    void* contents,
    size_t size,
    size_t nmemb,
    quoneq_http_response* response
) {
    size_t total_size = size * nmemb;
    std::string header_line(static_cast<char*>(contents), total_size);

    if(header_line.find("HTTP/") == 0) {
        std::istringstream iss(header_line);
        std::string protocol, status_code, status_text;
        iss >> protocol >> status_code >> status_text;

        response->statusType = status_text;
        response->status = static_cast<uint16_t>(
            std::stoi(status_code)
        );
    }

    size_t delimiter_pos = header_line.find(": ");
    if(delimiter_pos != std::string::npos) {
        std::string key = header_line.substr(0, delimiter_pos);
        std::string value = header_line.substr(delimiter_pos + 2);

        if(!value.empty() && value.back() == '\n')
            value.pop_back();
        if(!value.empty() && value.back() == '\r')
            value.pop_back();
        
        if(key == "Set-Cookie") {
            size_t semicolon_pos = value.find(';');
            std::string cookie_name = value.substr(0, value.find('='));
            std::string cookie_value = value.substr(
                value.find('=') + 1,
                semicolon_pos - value.find('=') - 1
            );
            response->cookies[cookie_name] = cookie_value;
        }
        else response->header[key] = value;
    }
    
    return total_size;
}

struct curl_slist* quoneq_http_client::prepare_headers(
    const std::map<std::string, std::string>& headers
) {
    struct curl_slist* curl_headers = nullptr;

    for(const auto& header : headers) {
        std::string header_str = header.first + ": " + header.second;
        curl_headers = curl_slist_append(
            curl_headers,
            header_str.c_str()
        );
    }
    return curl_headers;
}

std::string quoneq_http_client::prepare_cookies(
    const std::map<std::string, std::string>& cookies
) {
    std::string cookie_str;

    for(const auto& cookie : cookies) {
        if(!cookie_str.empty())
            cookie_str += "; ";
        cookie_str += cookie.first + "=" + cookie.second;
    }
    return cookie_str;
}

std::unique_ptr<quoneq_http_response> quoneq_http_client::get(
    const std::string& url,
    const std::map<std::string, std::string>& headers,
    const std::map<std::string, std::string>& cookies,
    const std::string& proxy,
    const std::string& username,
    const std::string& password
) {
    CURL* curl = curl_easy_init();
    if(!curl)
        return nullptr;

    auto response = std::make_unique<quoneq_http_response>();
    std::string response_string;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, quoneq_http_client::write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, quoneq_http_client::header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, response.get());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    struct curl_slist* curl_headers = quoneq_http_client::prepare_headers(headers);
    if(curl_headers)
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);

    std::string cookie_str = quoneq_http_client::prepare_cookies(cookies);
    if(!cookie_str.empty())
        curl_easy_setopt(curl, CURLOPT_COOKIE, cookie_str.c_str());

    if(!proxy.empty())
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());

    if(!username.empty() && !password.empty()) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(
            curl,
            CURLOPT_USERPWD,
            (username + ":" + password).c_str()
        );
    }

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        response->errorMessage = curl_easy_strerror(res);

        curl_slist_free_all(curl_headers);
        curl_easy_cleanup(curl);

        return response;
    }

    response->content = response_string;

    curl_slist_free_all(curl_headers);
    curl_easy_cleanup(curl);

    return response;
}

std::unique_ptr<quoneq_http_response> quoneq_http_client::post(
    const std::string& url,
    const std::map<std::string, std::string>& form,
    const std::map<std::string, std::string>& headers,
    const std::map<std::string, std::string>& cookies,
    const std::map<std::string, std::string>& files,
    const std::string& proxy,
    const std::string& username,
    const std::string& password
) {
    CURL* curl = curl_easy_init();
    if(!curl)
        return nullptr;

    auto response = std::make_unique<quoneq_http_response>();
    std::string response_string;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, quoneq_http_client::write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, quoneq_http_client::header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, response.get());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_mime* mime = curl_mime_init(curl);
    for(const auto& field : form) {
        curl_mimepart* part = curl_mime_addpart(mime);
        curl_mime_name(part, field.first.c_str());
        curl_mime_data(part, field.second.c_str(), CURL_ZERO_TERMINATED);
    }

    for(const auto& file : files) {
        curl_mimepart* part = curl_mime_addpart(mime);
        curl_mime_name(part, file.first.c_str());
        curl_mime_filedata(part, file.second.c_str());

        std::string filename = file.second.substr(
            file.second.find_last_of("/\\") + 1
        );
        curl_mime_filename(part, filename.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

    struct curl_slist* curl_headers = quoneq_http_client::prepare_headers(headers);
    if(curl_headers)
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);

    std::string cookie_str = quoneq_http_client::prepare_cookies(cookies);
    if(!cookie_str.empty())
        curl_easy_setopt(curl, CURLOPT_COOKIE, cookie_str.c_str());

    if(!proxy.empty())
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());

    if(!username.empty() && !password.empty()) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(
            curl,
            CURLOPT_USERPWD,
            (username + ":" + password).c_str()
        );
    }

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        response->errorMessage = curl_easy_strerror(res);

        curl_slist_free_all(curl_headers);
        curl_mime_free(mime);
        curl_easy_cleanup(curl);

        return response;
    }

    response->content = response_string;

    curl_slist_free_all(curl_headers);
    curl_mime_free(mime);
    curl_easy_cleanup(curl);

    return response;
}

std::unique_ptr<quoneq_http_response> quoneq_http_client::ping(
    const std::string& url,
    const std::string& proxy,
    const std::string& username,
    const std::string& password
) {
    CURL* curl = curl_easy_init();
    if(!curl)
        return nullptr;

    auto response = std::make_unique<quoneq_http_response>();
    std::string response_string;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, quoneq_http_client::write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, quoneq_http_client::header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, response.get());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

    if(!proxy.empty())
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());

    if(!username.empty() && !password.empty()) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(
            curl,
            CURLOPT_USERPWD,
            (username + ":" + password).c_str()
        );
    }

    auto start = std::chrono::high_resolution_clock::now();
    CURLcode res = curl_easy_perform(curl);
    auto end = std::chrono::high_resolution_clock::now();

    if(res == CURLE_OK) {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        response->status = static_cast<uint16_t>(response_code);

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        response->content = std::to_string(duration.count()) + " ms";
    }
    else {
        response->status = 0;
        response->content = curl_easy_strerror(res);
    }

    curl_easy_cleanup(curl);
    return response;
}

std::unique_ptr<quoneq_http_response> quoneq_http_client::download_file(
    const std::string& url,
    const std::string out_filename,
    const std::map<std::string, std::string>& form,
    const std::map<std::string, std::string>& headers,
    const std::map<std::string, std::string>& cookies,
    const std::map<std::string, std::string>& files,
    const std::string& proxy,
    const std::string& username,
    const std::string& password
) {
    CURL* curl = curl_easy_init();
    if(!curl)
        return nullptr;

    auto response = std::make_unique<quoneq_http_response>();
    std::ofstream output_file(out_filename, std::ios::binary);

    if(!output_file) {
        curl_easy_cleanup(curl);
        response->errorMessage = "Unable to open output file";
        return response;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, quoneq_http_client::write_file_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_file);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, quoneq_http_client::header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, response.get());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    struct curl_slist* header_list = quoneq_http_client::prepare_headers(headers);
    if(header_list)
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

    std::string cookie_str = quoneq_http_client::prepare_cookies(cookies);
    if(!cookie_str.empty())
        curl_easy_setopt(curl, CURLOPT_COOKIE, cookie_str.c_str());

    curl_mime* mime = nullptr;
    if(!form.empty() || !files.empty()) {
        mime = curl_mime_init(curl);
        for(const auto& field : form) {
            curl_mimepart* part = curl_mime_addpart(mime);
            curl_mime_name(part, field.first.c_str());
            curl_mime_data(part, field.second.c_str(), CURL_ZERO_TERMINATED);
        }

        for(const auto& file : files) {
            curl_mimepart* part = curl_mime_addpart(mime);
            curl_mime_name(part, file.first.c_str());
            curl_mime_filedata(part, file.second.c_str());

            std::string filename = file.second.substr(
                file.second.find_last_of("/\\") + 1
            );
            curl_mime_filename(part, filename.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    }

    if(!proxy.empty())
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());

    if(!username.empty() && !password.empty()) {
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(
            curl,
            CURLOPT_USERPWD,
            (username + ":" + password).c_str()
        );
    }

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    response->status = static_cast<uint16_t>(http_code);

    if(res != CURLE_OK)
        response->errorMessage = curl_easy_strerror(res);

    if(header_list)
        curl_slist_free_all(header_list);

    if(mime)
        curl_mime_free(mime);

    curl_easy_cleanup(curl);
    output_file.close();

    return response;
}
