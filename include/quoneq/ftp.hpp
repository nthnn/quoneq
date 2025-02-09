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
 * @file ftp.hpp
 * @author [Nathanne Isip](https://github.com/nthnn)
 * @brief Provides an FTP client library based on libcurl.
 *
 * This file defines the classes for performing various FTP operations such as
 * uploading, downloading, listing directories, removing, moving files, and
 * querying file/folder information using libcurl.
 */
#ifndef QUONEQ_FTP_HPP
#define QUONEQ_FTP_HPP

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <curl/curl.h>

/**
 * @brief Represents the response from an FTP operation.
 *
 * This class holds the response code, any error message, the returned content,
 * and, if applicable, a list of directory entries.
 */
typedef struct quoneq_ftp_response_t {
    long responseCode               = 0;    ///< The FTP response code.
    std::string errorMessage        = "";   ///< Any error message generated during the operation.
    std::string content             = "";   ///< The response content from the FTP operation.
    std::vector<std::string> list   = {};   ///< Directory listing when applicable.
} quoneq_ftp_response;

/**
 * @brief FTP client class providing static methods for FTP operations.
 *
 * This class wraps libcurl calls to implement FTP operations such as uploading,
 * downloading, reading, removing, listing directories (including recursive listing),
 * moving files, checking existence, and retrieving file/folder information.
 */
class quoneq_ftp_client {
private:
    /**
     * @brief Callback function used by libcurl to write received data into a string.
     *
     * @param ptr Pointer to the incoming data.
     * @param size Size of each data element.
     * @param nmemb Number of data elements.
     * @param data Pointer to the std::string to which data will be appended.
     * @return The total number of bytes processed.
     */
    static size_t write_callback(
        void* ptr,
        size_t size,
        size_t nmemb,
        std::string* data
    );

    /**
     * @brief Callback function used by libcurl to write data to a file stream.
     *
     * @param ptr Pointer to the incoming data.
     * @param size Size of each data element.
     * @param nmemb Number of data elements.
     * @param stream Pointer to the std::ofstream where data will be written.
     * @return The total number of bytes processed.
     */
    static size_t write_file_callback(
        void* ptr,
        size_t size,
        size_t nmemb,
        std::ofstream* stream
    );

    /**
     * @brief Callback function used by libcurl to read data from a file.
     *
     * @param ptr Pointer to the buffer where data should be stored.
     * @param size Size of each data element.
     * @param nmemb Number of data elements.
     * @param userdata Pointer to user data (typically a file stream).
     * @return The total number of bytes read.
     */
    static size_t read_file_callback(
        void* ptr,
        size_t size,
        size_t nmemb,
        void* userdata
    );

    /**
     * @brief Extracts the FTP path from a given FTP URL.
     *
     * @param ftp_url The complete FTP URL.
     * @return The FTP path extracted from the URL.
     */
    static std::string extract_ftp_path(const std::string &ftp_url);

    /**
     * @brief Splits a string into substrings based on a delimiter.
     *
     * @param s The input string.
     * @param delimiter The character to use as the delimiter.
     * @return A vector of substrings.
     */
    static std::vector<std::string> split_str(
        const std::string &s,
        char delimiter
    );

    /**
     * @brief Retrieves a detailed directory listing from an FTP server.
     *
     * @param ftp_url The FTP URL of the directory to list.
     * @param username FTP username.
     * @param password FTP password.
     * @return A vector of strings representing each line of the directory listing.
     */
    static std::vector<std::string> get_list_detail(
        const std::string &ftp_url,
        const std::string &username,
        const std::string &password
    );

    /**
     * @brief Parses a single line of an FTP directory listing.
     *
     * @param line A line from the FTP list output.
     * @return A pair where the first element indicates whether parsing was successful
     *         and the second element is the parsed detail string.
     */
    static std::pair<bool, std::string> parse_ftp_list_line(
        const std::string &line
    );

    /**
     * @brief Helper function to recursively list directory contents from an FTP server.
     *
     * This function accumulates directory listing entries into a provided vector.
     *
     * @param ftp_url The base FTP URL.
     * @param username FTP username.
     * @param password FTP password.
     * @param accum Reference to the vector accumulating directory listing entries.
     */
    static void list_recursive_helper(
        const std::string &ftp_url,
        const std::string &username,
        const std::string &password,
        std::vector<std::string> &accum
    );

public:
    /**
     * @brief Uploads a local file to the specified FTP server.
     *
     * @param ftp_url The FTP URL (including path) where the file should be uploaded.
     * @param local_file Path to the local file to be uploaded.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return A unique pointer to a quoneq_ftp_response containing the operation response.
     */
    static std::unique_ptr<quoneq_ftp_response> upload(
        const std::string &ftp_url,
        const std::string &local_file,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Downloads a file from the FTP server and saves it locally.
     *
     * @param ftp_url The FTP URL (including path) of the file to download.
     * @param local_file The local file path where the downloaded file will be saved.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return A unique pointer to a quoneq_ftp_response containing the operation response.
     */
    static std::unique_ptr<quoneq_ftp_response> download_file(
        const std::string &ftp_url,
        const std::string &local_file,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Reads the content of a file from the FTP server.
     *
     * @param ftp_url The FTP URL of the file to read.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return A unique pointer to a quoneq_ftp_response containing the file content.
     */
    static std::unique_ptr<quoneq_ftp_response> read(
        const std::string &ftp_url,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Removes a file from the FTP server.
     *
     * @param ftp_url The FTP URL of the file to remove.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return A unique pointer to a quoneq_ftp_response containing the operation response.
     */
    static std::unique_ptr<quoneq_ftp_response> remove(
        const std::string &ftp_url,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Lists the files and directories at the specified FTP URL.
     *
     * @param ftp_url The FTP URL of the directory to list.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return A unique pointer to a quoneq_ftp_response containing the directory list.
     */
    static std::unique_ptr<quoneq_ftp_response> list(
        const std::string &ftp_url,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Recursively lists all files and directories starting from the specified FTP URL.
     *
     * @param ftp_url The FTP URL of the root directory.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return A unique pointer to a quoneq_ftp_response containing the recursive directory list.
     */
    static std::unique_ptr<quoneq_ftp_response> list_recursive(
        const std::string &ftp_url,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Moves or renames a file or directory on the FTP server.
     *
     * @param ftp_url_from The source FTP URL.
     * @param ftp_url_to The destination FTP URL.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return A unique pointer to a quoneq_ftp_response containing the operation response.
     */
    static std::unique_ptr<quoneq_ftp_response> move(
        const std::string &ftp_url_from,
        const std::string &ftp_url_to,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Checks if a file or directory exists on the FTP server.
     *
     * @param ftp_url The FTP URL to check.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return true if the file or directory exists; false otherwise.
     */
    static bool exists(
        const std::string &ftp_url,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Determines if the specified FTP URL refers to a file.
     *
     * @param ftp_url The FTP URL to check.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return true if the FTP URL points to a file; false otherwise.
     */
    static bool is_file(
        const std::string &ftp_url,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Determines if the specified FTP URL refers to a folder.
     *
     * @param ftp_url The FTP URL to check.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return true if the FTP URL points to a folder; false otherwise.
     */
    static bool is_folder(
        const std::string &ftp_url,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Creates a directory on the FTP server.
     *
     * @param ftp_url The FTP URL where the directory will be created.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return A unique pointer to a quoneq_ftp_response containing the operation response.
     */
    static std::unique_ptr<quoneq_ftp_response> create(
        const std::string &ftp_url,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Retrieves information about a file on the FTP server.
     *
     * @param ftp_url The FTP URL of the file.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return A unique pointer to a quoneq_ftp_response containing the file information.
     */
    static std::unique_ptr<quoneq_ftp_response> file_info(
        const std::string &ftp_url,
        const std::string &username = "",
        const std::string &password = ""
    );

    /**
     * @brief Retrieves information about a folder on the FTP server.
     *
     * @param ftp_url The FTP URL of the folder.
     * @param username FTP username (optional).
     * @param password FTP password (optional).
     * @return A unique pointer to a quoneq_ftp_response containing the folder information.
     */
    static std::unique_ptr<quoneq_ftp_response> folder_info(
        const std::string &ftp_url,
        const std::string &username = "",
        const std::string &password = ""
    );
};

#endif
