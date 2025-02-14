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

#include <quoneq/ftp.hpp>

#include <curl/curl.h>
#include <fstream>

size_t quoneq_ftp_client::write_callback(
    void* ptr,
    size_t size,
    size_t nmemb,
    std::string* data
) {
    size_t total = size * nmemb;
    data->append(static_cast<char*>(ptr), total);

    return total;
}

size_t quoneq_ftp_client::write_file_callback(
    void* ptr,
    size_t size,
    size_t nmemb,
    std::ofstream* stream
) {
    size_t total = size * nmemb;
    stream->write(
        static_cast<char*>(ptr),
        static_cast<std::streamsize>(total)
    );

    return total;
}

size_t quoneq_ftp_client::read_file_callback(
    void* ptr, size_t size, size_t nmemb, void* userdata
) {
    std::ifstream* stream = static_cast<std::ifstream*>(userdata);
    if(!stream->good())
        return 0;

    stream->read(
        static_cast<char*>(ptr),
        static_cast<std::streamsize>(size * nmemb)
    );

    return static_cast<size_t>(stream->gcount());
}

std::string quoneq_ftp_client::extract_ftp_path(const std::string &ftp_url) {
    std::string path;
    size_t pos = ftp_url.find("ftp://");

    if(pos != std::string::npos) {
        pos = ftp_url.find('/', pos + 6);

        if(pos != std::string::npos)
            path = ftp_url.substr(pos);
    }

    return path;
}

std::vector<std::string> quoneq_ftp_client::split_str(
    const std::string &s,
    char delimiter
) {
    std::vector<std::string> elems;
    std::istringstream iss(s);
    std::string item;

    while(std::getline(iss, item, delimiter)) {
        if(!item.empty())
            elems.push_back(item);
    }

    return elems;
}

std::vector<std::string> quoneq_ftp_client::get_list_detail(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password
) {
    std::vector<std::string> lines;
    CURL* curl = curl_easy_init();

    if(!curl)
        return lines;

    std::string data;
    curl_easy_setopt(curl, CURLOPT_URL, ftp_url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        quoneq_ftp_client::write_callback
    );

    if(!username.empty())
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());

    if(!password.empty())
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if(res == CURLE_OK)
        lines = quoneq_ftp_client::split_str(data, '\n');

    return lines;
}

std::pair<bool, std::string> quoneq_ftp_client::parse_ftp_list_line(
    const std::string &line
) {
    if(line.empty())
        return {false, ""};

    bool isDir = (line[0] == 'd');
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;

    while(iss >> token)
        tokens.push_back(token);

    std::string name;
    if(tokens.size() >= 9) {
        name = tokens[8];
        for(size_t i = 9; i < tokens.size(); i++)
            name += " " + tokens[i];
    }
    else if(!tokens.empty())
        name = tokens.back();

    return {isDir, name};
}

void quoneq_ftp_client::list_recursive_helper(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password,
    std::vector<std::string> &accum
) {
    auto lines = quoneq_ftp_client::get_list_detail(
        ftp_url,
        username,
        password
    );
    for(const auto &line : lines) {
        auto parsed = quoneq_ftp_client::parse_ftp_list_line(line);
        bool isDir = parsed.first;

        std::string name = parsed.second;
        if(name == "." || name == ".." || name.empty())
            continue;

        std::string fullPath = ftp_url;
        if(fullPath.back() != '/')
            fullPath += "/";

        fullPath += name;
        accum.push_back(fullPath);

        if(isDir)
            quoneq_ftp_client::list_recursive_helper(
                fullPath,
                username,
                password,
                accum
            );
    }
}

std::unique_ptr<quoneq_ftp_response> quoneq_ftp_client::upload(
    const std::string &ftp_url,
    const std::string &local_file,
    const std::string &username,
    const std::string &password
) {
    auto response = std::make_unique<quoneq_ftp_response>();
    CURL* curl = curl_easy_init();

    if(!curl) {
        response->errorMessage = "Failed to initialize curl";
        return response;
    }

    std::ifstream file(local_file, std::ios::binary);
    if(!file.is_open()) {
        response->errorMessage = "Unable to open local file for reading";
        curl_easy_cleanup(curl);

        return response;
    }

    file.seekg(0, std::ios::end);

    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, ftp_url.c_str());
    curl_easy_setopt(curl, CURLOPT_READDATA, &file);
    curl_easy_setopt(
        curl,
        CURLOPT_READFUNCTION,
        quoneq_ftp_client::read_file_callback
    );
    curl_easy_setopt(
        curl,
        CURLOPT_INFILESIZE_LARGE,
        static_cast<curl_off_t>(fileSize)
    );

    if(!username.empty())
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());

    if(!password.empty())
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        response->errorMessage = curl_easy_strerror(res);
    else curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &response->responseCode
    );

    curl_easy_cleanup(curl);
    file.close();

    return response;
}

std::unique_ptr<quoneq_ftp_response> quoneq_ftp_client::download_file(
    const std::string &ftp_url,
    const std::string &local_file,
    const std::string &username,
    const std::string &password
) {
    auto response = std::make_unique<quoneq_ftp_response>();
    CURL* curl = curl_easy_init();

    if(!curl) {
        response->errorMessage = "Failed to initialize curl";
        return response;
    }

    std::ofstream outfile(local_file, std::ios::binary);
    if(!outfile.is_open()) {
        response->errorMessage = "Unable to open local file for writing";
        curl_easy_cleanup(curl);

        return response;
    }

    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_URL, ftp_url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outfile);
    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        quoneq_ftp_client::write_file_callback
    );

    if(!username.empty())
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());

    if(!password.empty())
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        response->errorMessage = curl_easy_strerror(res);
    else curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &response->responseCode
    );

    curl_easy_cleanup(curl);
    outfile.close();

    return response;
}

std::unique_ptr<quoneq_ftp_response> quoneq_ftp_client::read(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password
) {
    auto response = std::make_unique<quoneq_ftp_response>();
    CURL* curl = curl_easy_init();

    if(!curl) {
        response->errorMessage = "Failed to initialize curl";
        return response;
    }

    std::string data;
    curl_easy_setopt(curl, CURLOPT_URL, ftp_url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        quoneq_ftp_client::write_callback
    );

    if(!username.empty())
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());

    if(!password.empty())
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        response->errorMessage = curl_easy_strerror(res);
    else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->responseCode);
        response->content = data;
    }

    curl_easy_cleanup(curl);
    return response;
}

std::unique_ptr<quoneq_ftp_response> quoneq_ftp_client::remove(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password
) {
    auto response = std::make_unique<quoneq_ftp_response>();
    CURL* curl = curl_easy_init();

    if(!curl) {
        response->errorMessage = "Failed to initialize curl";
        return response;
    }

    curl_easy_setopt(curl, CURLOPT_URL, ftp_url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

    std::string path = quoneq_ftp_client::extract_ftp_path(ftp_url);
    std::string deleCmd = "DELE " + path;

    struct curl_slist* cmdList = nullptr;
    cmdList = curl_slist_append(cmdList, deleCmd.c_str());
    curl_easy_setopt(curl, CURLOPT_QUOTE, cmdList);

    if(!username.empty())
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());

    if(!password.empty())
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        response->errorMessage = curl_easy_strerror(res);
    else curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &response->responseCode
    );

    curl_slist_free_all(cmdList);
    curl_easy_cleanup(curl);

    return response;
}

std::unique_ptr<quoneq_ftp_response> quoneq_ftp_client::list(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password
) {
    auto response = std::make_unique<quoneq_ftp_response>();
    CURL* curl = curl_easy_init();

    if(!curl) {
        response->errorMessage = "Failed to initialize curl";
        return response;
    }

    std::string data;
    curl_easy_setopt(curl, CURLOPT_URL, ftp_url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1L);
    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        quoneq_ftp_client::write_callback
    );

    if(!username.empty())
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());

    if(!password.empty())
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        response->errorMessage = curl_easy_strerror(res);
    else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->responseCode);
        response->content = data;
        response->list = quoneq_ftp_client::split_str(data, '\n');
    }

    curl_easy_cleanup(curl);
    return response;
}

std::unique_ptr<quoneq_ftp_response> quoneq_ftp_client::list_recursive(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password
) {
    auto response = std::make_unique<quoneq_ftp_response>();
    std::vector<std::string> accum;

    list_recursive_helper(ftp_url, username, password, accum);
    response->list = accum;

    return response;
}

std::unique_ptr<quoneq_ftp_response> quoneq_ftp_client::move(
    const std::string &ftp_url_from,
    const std::string &ftp_url_to,
    const std::string &username,
    const std::string &password
) {
    auto response = std::make_unique<quoneq_ftp_response>();
    CURL* curl = curl_easy_init();

    if(!curl) {
        response->errorMessage = "Failed to initialize curl";
        return response;
    }
    curl_easy_setopt(curl, CURLOPT_URL, ftp_url_from.c_str());

    std::string path_from = quoneq_ftp_client::extract_ftp_path(ftp_url_from);
    std::string path_to = quoneq_ftp_client::extract_ftp_path(ftp_url_to);
    std::string rnfrCmd = "RNFR " + path_from;
    std::string rntoCmd = "RNTO " + path_to;

    struct curl_slist* cmdList = nullptr;
    cmdList = curl_slist_append(cmdList, rnfrCmd.c_str());
    cmdList = curl_slist_append(cmdList, rntoCmd.c_str());
    curl_easy_setopt(curl, CURLOPT_QUOTE, cmdList);

    if(!username.empty())
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());

    if(!password.empty())
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        response->errorMessage = curl_easy_strerror(res);
    else curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &response->responseCode
    );

    curl_slist_free_all(cmdList);
    curl_easy_cleanup(curl);

    return response;
}

bool quoneq_ftp_client::exists(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password
) {
    CURL* curl = curl_easy_init();

    if(!curl)
        return false;

    curl_easy_setopt(curl, CURLOPT_URL, ftp_url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

    if(!username.empty())
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());

    if(!password.empty())
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK);
}

bool quoneq_ftp_client::is_file(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password
) {
    auto info = file_info(ftp_url, username, password);
    if(!info->content.empty() && info->content[0] == '-')
        return true;

    return false;
}

bool quoneq_ftp_client::is_folder(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password
) {
    auto info = folder_info(ftp_url, username, password);
    return !info->content.empty() && info->content[0] == 'd';
}

std::unique_ptr<quoneq_ftp_response> quoneq_ftp_client::create(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password
) {
    auto response = std::make_unique<quoneq_ftp_response>();
    CURL* curl = curl_easy_init();

    if(!curl) {
        response->errorMessage = "Failed to initialize curl";
        return response;
    }

    curl_easy_setopt(curl, CURLOPT_URL, ftp_url.c_str());
    std::string path = quoneq_ftp_client::extract_ftp_path(ftp_url);
    std::string mkdCmd = "MKD " + path;

    struct curl_slist* cmdList = nullptr;
    cmdList = curl_slist_append(cmdList, mkdCmd.c_str());
    curl_easy_setopt(curl, CURLOPT_QUOTE, cmdList);

    if(!username.empty())
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());

    if(!password.empty())
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        response->errorMessage = curl_easy_strerror(res);
    else curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &response->responseCode
    );

    curl_slist_free_all(cmdList);
    curl_easy_cleanup(curl);

    return response;
}

std::unique_ptr<quoneq_ftp_response> quoneq_ftp_client::file_info(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password
) {
    auto response = std::make_unique<quoneq_ftp_response>();
    CURL* curl = curl_easy_init();

    if(!curl) {
        response->errorMessage = "Failed to initialize curl";
        return response;
    }

    std::string data;
    curl_easy_setopt(curl, CURLOPT_URL, ftp_url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        quoneq_ftp_client::write_callback
    );

    if(!username.empty())
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());

    if(!password.empty())
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        response->errorMessage = curl_easy_strerror(res);
    else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->responseCode);
        response->content = data;
    }

    curl_easy_cleanup(curl);
    return response;
}

std::unique_ptr<quoneq_ftp_response> quoneq_ftp_client::folder_info(
    const std::string &ftp_url,
    const std::string &username,
    const std::string &password
) {
    auto response = std::make_unique<quoneq_ftp_response>();
    CURL* curl = curl_easy_init();

    if(!curl) {
        response->errorMessage = "Failed to initialize curl";
        return response;
    }

    std::string data;
    curl_easy_setopt(curl, CURLOPT_URL, ftp_url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        quoneq_ftp_client::write_callback
    );

    if(!username.empty())
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());

    if(!password.empty())
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        response->errorMessage = curl_easy_strerror(res);
    else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->responseCode);
        response->content = data;
    }

    curl_easy_cleanup(curl);
    return response;
}
