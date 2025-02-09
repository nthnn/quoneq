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
 * @file quoneq_telnet.hpp
 * @author [Nathanne Isip](https://github.com/nthnn)
 * @brief Provides a Telnet client library based on libcurl.
 *
 * This header defines the classes used for performing Telnet operations using libcurl.
 * It includes functionality to connect to a Telnet server, send one or more commands,
 * execute a Telnet script from a file, and send Telnet commands with additional Telnet options.
 */
#ifndef QUONEQ_TELNET_HPP
#define QUONEQ_TELNET_HPP

#include <chrono>
#include <cstdint>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <curl/curl.h>

/**
 * @brief Represents a response from a Telnet operation.
 *
 * This class contains the error message (if any) and the content received from the Telnet server.
 */
typedef struct quoneq_telnet_response_t {
    std::string error_message   = "";   ///< Error message returned from the Telnet operation, if any.
    std::string content         = "";   ///< Content received from the Telnet server.
} quoneq_telnet_response;

/**
 * @brief Telnet client class using libcurl.
 *
 * The quoneq_telnet_client class provides static methods to perform various Telnet operations.
 * It supports:
 *  - Executing a series of Telnet commands.
 *  - Connecting to a Telnet server to receive an initial prompt.
 *  - Sending a single Telnet command.
 *  - Executing a Telnet script from a file.
 *  - Executing Telnet commands along with additional Telnet options.
 */
class quoneq_telnet_client {
private:
    /**
     * @brief Callback function for writing data received from the Telnet server.
     *
     * This function is called by libcurl to write data into a std::string.
     *
     * @param contents Pointer to the incoming data.
     * @param size Size of each element.
     * @param nmemb Number of elements.
     * @param output Pointer to the std::string that will receive the data.
     * @return The total number of bytes processed.
     */
    static size_t write_callback(
        void* contents,
        size_t size,
        size_t nmemb,
        std::string* output
    );

public:
    /**
     * @brief Executes one or more Telnet commands.
     *
     * This method connects to the specified Telnet server URL and sends a series of commands.
     * It supports optional proxy configuration, basic authentication, and a connection timeout.
     *
     * @param url The Telnet URL to connect to.
     * @param commands A vector of commands to execute on the Telnet server.
     * @param proxy (Optional) The proxy server to use.
     * @param username (Optional) Username for authentication.
     * @param password (Optional) Password for authentication.
     * @param timeout (Optional) Connection timeout in seconds.
     * @return A unique pointer to a quoneq_telnet_response containing the server's response.
     */
    static std::unique_ptr<quoneq_telnet_response> command(
        const std::string& url,
        const std::vector<std::string>& commands,
        const std::string& proxy = "",
        const std::string& username = "",
        const std::string& password = "",
        long timeout = 30L
    );

    /**
     * @brief Connects to a Telnet server and retrieves the initial response.
     *
     * This method connects to the specified Telnet server URL without sending additional commands,
     * allowing the client to retrieve the initial banner or prompt.
     *
     * @param url The Telnet URL to connect to.
     * @param proxy (Optional) The proxy server to use.
     * @param username (Optional) Username for authentication.
     * @param password (Optional) Password for authentication.
     * @param timeout (Optional) Connection timeout in seconds.
     * @return A unique pointer to a quoneq_telnet_response containing the initial response.
     */
    static std::unique_ptr<quoneq_telnet_response> connect(
        const std::string& url,
        const std::string& proxy = "",
        const std::string& username = "",
        const std::string& password = "",
        long timeout = 30L
    );

    /**
     * @brief Sends a single Telnet command.
     *
     * This method connects to the specified Telnet server and sends a single command.
     *
     * @param url The Telnet URL to connect to.
     * @param command The command to send to the Telnet server.
     * @param proxy (Optional) The proxy server to use.
     * @param username (Optional) Username for authentication.
     * @param password (Optional) Password for authentication.
     * @param timeout (Optional) Connection timeout in seconds.
     * @return A unique pointer to a quoneq_telnet_response containing the server's response.
     */
    static std::unique_ptr<quoneq_telnet_response> quote(
        const std::string& url,
        const std::string& command,
        const std::string& proxy = "",
        const std::string& username = "",
        const std::string& password = "",
        long timeout = 30L
    );

    /**
     * @brief Executes a Telnet script from a file.
     *
     * This method reads a file containing Telnet commands (one per line) and sends them sequentially
     * to the Telnet server.
     *
     * @param url The Telnet URL to connect to.
     * @param script_filename The path to the file containing the Telnet commands.
     * @param proxy (Optional) The proxy server to use.
     * @param username (Optional) Username for authentication.
     * @param password (Optional) Password for authentication.
     * @param timeout (Optional) Connection timeout in seconds.
     * @return A unique pointer to a quoneq_telnet_response containing the server's response.
     */
    static std::unique_ptr<quoneq_telnet_response> script(
        const std::string& url,
        const std::string& script_filename,
        const std::string& proxy = "",
        const std::string& username = "",
        const std::string& password = "",
        long timeout = 30L
    );

    /**
     * @brief Executes Telnet commands with additional Telnet options.
     *
     * This method sends Telnet-specific options (such as negotiation commands) along with
     * a series of Telnet commands.
     *
     * @param url The Telnet URL to connect to.
     * @param telnet_options A vector of Telnet options to be sent.
     * @param commands A vector of Telnet commands to execute.
     * @param proxy (Optional) The proxy server to use.
     * @param username (Optional) Username for authentication.
     * @param password (Optional) Password for authentication.
     * @param timeout (Optional) Connection timeout in seconds.
     * @return A unique pointer to a quoneq_telnet_response containing the server's response.
     */
    static std::unique_ptr<quoneq_telnet_response> exec_with_options(
        const std::string& url,
        const std::vector<std::string>& telnet_options,
        const std::vector<std::string>& commands,
        const std::string& proxy = "",
        const std::string& username = "",
        const std::string& password = "",
        long timeout = 30L
    );
};

#endif
