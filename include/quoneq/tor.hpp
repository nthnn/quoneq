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
 * @file quoneq_tor.hpp
 * @author [Nathanne Isip](https://github.com/nthnn)
 * @brief Provides a Tor-enabled HTTP client using the Quoneq HTTP library.
 *
 * This header defines the quoneq_tor_client class, which extends the functionality of
 * the standard HTTP client (quoneq_http_client) to route HTTP requests through the Tor network.
 * It supports GET, POST, ping requests, checking Tor connectivity, and file downloads over Tor.
 */
#ifndef QUONEQ_TOR_HPP
#define QUONEQ_TOR_HPP

#include <quoneq/http.hpp>

/**
 * @brief Tor-enabled HTTP client.
 *
 * The quoneq_tor_client class provides static methods to perform HTTP operations
 * via the Tor network. These methods wrap the underlying HTTP client calls and
 * configure them to use Tor as the transport layer.
 */
class quoneq_tor_client {
public:
    /**
     * @brief Sends an HTTP GET request over Tor.
     *
     * This method sends an HTTP GET request to the specified URL using the Tor network.
     * Optional parameters allow for setting custom headers, cookies, and basic authentication.
     *
     * @param url The target URL.
     * @param headers (Optional) A map of header fields and their corresponding values.
     * @param cookies (Optional) A map of cookie names and values.
     * @param username (Optional) Username for basic authentication.
     * @param password (Optional) Password for basic authentication.
     * @return A unique pointer to a quoneq_http_response containing the response data.
     */
    static std::unique_ptr<quoneq_http_response> get(
        const std::string& url, 
        const std::map<std::string, std::string>& headers = {}, 
        const std::map<std::string, std::string>& cookies = {},
        const std::string& username = "", 
        const std::string& password = ""
    );

    /**
     * @brief Sends an HTTP POST request over Tor.
     *
     * This method sends an HTTP POST request to the specified URL using the Tor network.
     * It supports sending form data, custom headers, cookies, and file uploads, as well as
     * basic authentication.
     *
     * @param url The target URL.
     * @param form (Optional) A map of form field names and values.
     * @param headers (Optional) A map of header fields and their corresponding values.
     * @param cookies (Optional) A map of cookie names and values.
     * @param files (Optional) A map of file form field names and their associated file paths.
     * @param username (Optional) Username for basic authentication.
     * @param password (Optional) Password for basic authentication.
     * @return A unique pointer to a quoneq_http_response containing the response data.
     */
    static std::unique_ptr<quoneq_http_response> post(
        const std::string& url,
        const std::map<std::string, std::string>& form = {},
        const std::map<std::string, std::string>& headers = {},
        const std::map<std::string, std::string>& cookies = {},
        const std::map<std::string, std::string>& files = {},
        const std::string& username = "",
        const std::string& password = ""
    );

    /**
     * @brief Pings a URL over Tor to check connectivity.
     *
     * This method sends an HTTP request (without a message body) to the specified URL using Tor.
     * It is used to verify that the target URL is reachable via the Tor network.
     *
     * @param url The target URL to ping.
     * @param username (Optional) Username for basic authentication.
     * @param password (Optional) Password for basic authentication.
     * @return A unique pointer to a quoneq_http_response containing the ping response.
     */
    static std::unique_ptr<quoneq_http_response> ping(
        const std::string& url, 
        const std::string& username = "", 
        const std::string& password = ""
    );

    /**
     * @brief Checks if the Tor network is running.
     *
     * This method attempts to determine whether the Tor network is active by using a
     * predefined URL or check routine. It returns true if Tor is running and accessible,
     * and false otherwise.
     *
     * @return True if Tor is running; false otherwise.
     */
    static bool is_tor_running();

    /**
     * @brief Downloads a file over Tor.
     *
     * This method downloads the file located at the specified URL via the Tor network and
     * saves it to the provided local file name. It supports optional form data, custom headers,
     * cookies, file uploads, and basic authentication.
     *
     * @param url The URL of the file to download.
     * @param out_filename The local file name where the downloaded file will be saved.
     * @param form (Optional) A map of form field names and values.
     * @param headers (Optional) A map of header fields and their corresponding values.
     * @param cookies (Optional) A map of cookie names and values.
     * @param files (Optional) A map of file form field names and their associated file paths.
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
        const std::string& username = "",
        const std::string& password = ""
    );
};

#endif
