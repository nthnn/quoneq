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

#include <quoneq/telnet.hpp>

#include <curl/curl.h>

size_t quoneq_telnet_client::write_callback(
    void* contents,
    size_t size,
    size_t nmemb,
    std::string* output
) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);

    return total_size;
}

std::unique_ptr<quoneq_telnet_response> quoneq_telnet_client::command(
    const std::string& url,
    const std::vector<std::string>& commands,
    const std::string& proxy,
    const std::string& username,
    const std::string& password,
    long timeout
) {
    CURL* curl = curl_easy_init();
    if(!curl)
        return nullptr;

    auto response = std::make_unique<quoneq_telnet_response>();
    std::string response_string;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        quoneq_telnet_client::write_callback
    );

    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);

    if(!proxy.empty())
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());

    if(!username.empty() && !password.empty()) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
    }

    struct curl_slist* command_list = nullptr;
    for(const auto& cmd : commands)
        command_list = curl_slist_append(command_list, cmd.c_str());

    if(command_list)
        curl_easy_setopt(curl, CURLOPT_QUOTE, command_list);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        response->error_message = curl_easy_strerror(res);

    response->content = response_string;

    if(command_list)
        curl_slist_free_all(command_list);

    curl_easy_cleanup(curl);
    return response;
}

std::unique_ptr<quoneq_telnet_response> quoneq_telnet_client::connect(
    const std::string& url,
    const std::string& proxy,
    const std::string& username,
    const std::string& password,
    long timeout
) {
    std::vector<std::string> empty_commands;
    return quoneq_telnet_client::command(
        url,
        empty_commands,
        proxy,
        username,
        password,
        timeout
    );
}

std::unique_ptr<quoneq_telnet_response> quoneq_telnet_client::quote(
    const std::string& url,
    const std::string& command,
    const std::string& proxy,
    const std::string& username,
    const std::string& password,
    long timeout
) {
    std::vector<std::string> commands = { command };
    return quoneq_telnet_client::command(
        url,
        commands,
        proxy,
        username,
        password,
        timeout
    );
}

std::unique_ptr<quoneq_telnet_response> quoneq_telnet_client::script(
    const std::string& url,
    const std::string& script_filename,
    const std::string& proxy,
    const std::string& username,
    const std::string& password,
    long timeout
) {
    std::ifstream script_file(script_filename);
    auto response = std::make_unique<quoneq_telnet_response>();

    if(!script_file.is_open()) {
        response->error_message = "unable to open script file: " + script_filename;
        return response;
    }

    std::vector<std::string> commands;
    std::string line;

    while(std::getline(script_file, line)) {
        if(!line.empty())
            commands.push_back(line);
    }

    return quoneq_telnet_client::command(
        url,
        commands,
        proxy,
        username,
        password,
        timeout
    );
}

std::unique_ptr<quoneq_telnet_response> quoneq_telnet_client::exec_with_options(
    const std::string& url,
    const std::vector<std::string>& telnet_options,
    const std::vector<std::string>& commands,
    const std::string& proxy,
    const std::string& username,
    const std::string& password,
    long timeout
) {
    CURL* curl = curl_easy_init();
    if(!curl)
        return nullptr;

    auto response = std::make_unique<quoneq_telnet_response>();
    std::string response_string;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        quoneq_telnet_client::write_callback
    );

    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);

    if(!proxy.empty())
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());

    if(!username.empty() && !password.empty()) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
    }

    struct curl_slist* telnet_options_list = nullptr;
    for(const auto& opt : telnet_options)
        telnet_options_list = curl_slist_append(
            telnet_options_list, opt.c_str()
        );

    if(telnet_options_list)
        curl_easy_setopt(curl, CURLOPT_TELNETOPTIONS, telnet_options_list);

    struct curl_slist* command_list = nullptr;
    for(const auto& cmd : commands)
        command_list = curl_slist_append(command_list, cmd.c_str());

    if(command_list)
        curl_easy_setopt(curl, CURLOPT_QUOTE, command_list);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
        response->error_message = curl_easy_strerror(res);
    response->content = response_string;

    if(telnet_options_list)
        curl_slist_free_all(telnet_options_list);

    if(command_list)
        curl_slist_free_all(command_list);

    curl_easy_cleanup(curl);
    return response;
}
