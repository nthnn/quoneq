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

/**
 * @file quoneq_http.hpp
 * @author [Nathanne Isip](https://github.com/nthnn)
 * @brief Provides an HTTP client library based on libcurl.
 *
 * This header defines classes for performing HTTP requests (GET, POST, ping, and file download)
 * using libcurl. It supports setting custom headers, cookies, proxy configurations, and basic authentication.
 */
#ifndef QUONEQ_HTTP_HPP
#define QUONEQ_HTTP_HPP

#include <map>
#include <memory>
#include <string>

#include <curl/curl.h>

/**
 * @brief Represents an HTTP response.
 *
 * This class holds various pieces of information returned by an HTTP request,
 * including the status code, status text, any error messages, the response content,
 * as well as parsed headers and cookies.
 */
typedef struct quoneq_http_response_t {
    uint16_t status                             = 0;    ///< HTTP status code.
    std::string statusType                      = "";   ///< HTTP status text (e.g., "OK").
    std::string errorMessage                    = "";   ///< Error message, if any.
    std::string content                         = "";   ///< The response body content.
    std::map<std::string, std::string> header   = {};   ///< Map of HTTP response header fields.
    std::map<std::string, std::string> cookies  = {};   ///< Map of cookies received in the response.
} quoneq_http_response;

/**
 * @brief HTTP client for performing HTTP operations using libcurl.
 *
 * The quoneq_http_client class provides a collection of static methods for executing HTTP
 * requests such as GET, POST, pinging a URL, and downloading files. It supports custom headers,
 * cookies, proxy settings, and basic authentication.
 */
class quoneq_http_client {
private:
    /**
     * @brief Callback function used by libcurl to write received data into a string.
     *
     * This function appends the data received from libcurl to the provided std::string.
     *
     * @param contents Pointer to the incoming data.
     * @param size Size of each data element.
     * @param nmemb Number of data elements.
     * @param output Pointer to the std::string that receives the data.
     * @return The number of bytes processed.
     */
    static size_t write_callback(
        void* contents,
        size_t size,
        size_t nmemb,
        std::string* output
    );

    /**
     * @brief Callback function used by libcurl to write received data to a file stream.
     *
     * This function writes the incoming data to an std::ofstream.
     *
     * @param contents Pointer to the incoming data.
     * @param size Size of each data element.
     * @param nmemb Number of data elements.
     * @param file Pointer to the std::ofstream where the data will be written.
     * @return The number of bytes processed.
     */
    static size_t write_file_callback(
        void* contents,
        size_t size,
        size_t nmemb,
        std::ofstream* file
    );

    /**
     * @brief Callback function used by libcurl to process HTTP header data.
     *
     * This function is called as header data is received and it extracts
     * the HTTP status and headers, populating the quoneq_http_response object.
     *
     * @param contents Pointer to the header data.
     * @param size Size of each element.
     * @param nmemb Number of elements.
     * @param response Pointer to the quoneq_http_response object to populate.
     * @return The number of bytes processed.
     */
    static size_t header_callback(
        void* contents,
        size_t size,
        size_t nmemb,
        quoneq_http_response* response
    );

    /**
     * @brief Prepares a libcurl header list from a map of header key-value pairs.
     *
     * This function converts the provided map of headers into a curl_slist that can be used
     * with libcurl.
     *
     * @param headers Map of header fields and values.
     * @return A pointer to a curl_slist containing the formatted headers.
     */
    static struct curl_slist* prepare_headers(
        const std::map<std::string, std::string>& headers
    );

    /**
     * @brief Prepares a cookie string from a map of cookie names and values.
     *
     * This function concatenates cookies from the map into a single string formatted
     * for libcurl's CURLOPT_COOKIE option.
     *
     * @param cookies Map of cookie names and values.
     * @return A string containing the formatted cookies.
     */
    static std::string prepare_cookies(
        const std::map<std::string, std::string>& cookies
    );

public:
    /**
     * @brief Sends an HTTP GET request.
     *
     * This method performs an HTTP GET request to the specified URL, with optional custom headers,
     * cookies, proxy, and basic authentication.
     *
     * @param url The target URL.
     * @param headers (Optional) Map of HTTP headers to include in the request.
     * @param cookies (Optional) Map of cookies to include in the request.
     * @param proxy (Optional) Proxy server to use.
     * @param username (Optional) Username for basic authentication.
     * @param password (Optional) Password for basic authentication.
     * @return A unique pointer to a quoneq_http_response containing the response.
     */
    static std::unique_ptr<quoneq_http_response> get(
        const std::string& url, 
        const std::map<std::string, std::string>& headers = {}, 
        const std::map<std::string, std::string>& cookies = {}, 
        const std::string& proxy = "", 
        const std::string& username = "", 
        const std::string& password = ""
    );

    /**
     * @brief Sends an HTTP POST request.
     *
     * This method performs an HTTP POST request to the specified URL. It supports sending form data,
     * custom headers, cookies, file uploads, proxy configuration, and basic authentication.
     *
     * @param url The target URL.
     * @param form (Optional) Map of form fields and values.
     * @param headers (Optional) Map of HTTP headers.
     * @param cookies (Optional) Map of cookies.
     * @param files (Optional) Map of file form fields and corresponding file paths.
     * @param proxy (Optional) Proxy server to use.
     * @param username (Optional) Username for basic authentication.
     * @param password (Optional) Password for basic authentication.
     * @return A unique pointer to a quoneq_http_response containing the response.
     */
    static std::unique_ptr<quoneq_http_response> post(
        const std::string& url,
        const std::map<std::string, std::string>& form = {},
        const std::map<std::string, std::string>& headers = {},
        const std::map<std::string, std::string>& cookies = {},
        const std::map<std::string, std::string>& files = {},
        const std::string& proxy = "",
        const std::string& username = "",
        const std::string& password = ""
    );

    /**
     * @brief Pings a URL to check connectivity.
     *
     * This method sends an HTTP request without a body (or with the NOBODY option)
     * to verify that the target URL is reachable. It returns the response code and any response details.
     *
     * @param url The target URL to ping.
     * @param proxy (Optional) Proxy server to use.
     * @param username (Optional) Username for basic authentication.
     * @param password (Optional) Password for basic authentication.
     * @return A unique pointer to a quoneq_http_response containing the ping response.
     */
    static std::unique_ptr<quoneq_http_response> ping(
        const std::string& url, 
        const std::string& proxy = "", 
        const std::string& username = "", 
        const std::string& password = ""
    );

    /**
     * @brief Downloads a file from the specified URL.
     *
     * This method downloads the file located at the given URL and saves it to a local file.
     * It supports additional options such as form data, custom headers, cookies, file uploads,
     * proxy configuration, and basic authentication.
     *
     * @param url The URL of the file to download.
     * @param out_filename The local filename where the downloaded file will be saved.
     * @param form (Optional) Map of form fields and values.
     * @param headers (Optional) Map of HTTP headers.
     * @param cookies (Optional) Map of cookies.
     * @param files (Optional) Map of file form fields and corresponding file paths.
     * @param proxy (Optional) Proxy server to use.
     * @param username (Optional) Username for basic authentication.
     * @param password (Optional) Password for basic authentication.
     * @return A unique pointer to a quoneq_http_response containing the file download response.
     */
    static std::unique_ptr<quoneq_http_response> download_file(
        const std::string& url,
        const std::string out_filename,
        const std::map<std::string, std::string>& form = {},
        const std::map<std::string, std::string>& headers = {},
        const std::map<std::string, std::string>& cookies = {},
        const std::map<std::string, std::string>& files = {},
        const std::string& proxy = "",
        const std::string& username = "",
        const std::string& password = ""
    );
};

#endif
