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

#include <quoneq/smtp.hpp>

#include <cstring>

size_t quoneq_smtp_client::payload_source(
    void* ptr,
    size_t size,
    size_t nmemb,
    void* userp
) {
    upload_context* upload = static_cast<upload_context*>(userp);
    const size_t buffer_size = size * nmemb;
    const size_t copy_size = std::min(
        upload->payload.size() -
            upload->bytes_read,
        buffer_size
    );

    if(copy_size) {
        memcpy(
            ptr,
            upload->payload.data() +
                upload->bytes_read, copy_size
        );

        upload->bytes_read += copy_size;
        return copy_size;
    }

    return 0;
}

void quoneq_smtp_client::setup_mime_structure(
    CURL* curl,
    const std::string& email,
    const std::string& recipient,
    const std::string& subject,
    const std::string& message,
    bool is_html,
    const std::vector<std::string>& files
) {
    curl_mime* mime = curl_mime_init(curl);
    const std::string boundary = "QUONEQ-BOUNDARY";

    curl_mimepart* root_part = curl_mime_addpart(mime);
    std::string content_type = "multipart/mixed; boundary=\"" +
        boundary + "\"";
    curl_mime_type(root_part, content_type.c_str());

    struct curl_slist* headers = NULL;

    headers = curl_slist_append(headers, ("From: " + email).c_str());
    headers = curl_slist_append(headers, ("To: " + recipient).c_str());
    headers = curl_slist_append(headers, ("Subject: " + subject).c_str());
    headers = curl_slist_append(headers, "MIME-Version: 1.0");
    curl_mime_headers(root_part, headers, 1);

    curl_mimepart* body_part = curl_mime_addpart(mime);
    std::string body_type = is_html ? "text/html" : "text/plain";

    curl_mime_type(body_part, body_type.c_str());
    curl_mime_data(body_part, message.c_str(), message.size());

    for(const auto& file : files) {
        curl_mimepart* attach_part = curl_mime_addpart(mime);

        curl_mime_filedata(attach_part, file.c_str());
        curl_mime_type(attach_part, "application/octet-stream");
        curl_mime_encoder(attach_part, "base64");
        
        size_t last_slash = file.find_last_of("/\\");
        std::string filename = (last_slash != std::string::npos) 
            ? file.substr(last_slash + 1)
            : file;
        curl_mime_filename(attach_part, filename.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
}

bool quoneq_smtp_client::send_email(
    const std::string& smtp_server,
    const std::string& email,
    const std::string& password,
    const std::string& recipient,
    const std::string& subject,
    const std::string& message,
    bool is_html,
    const std::vector<std::string>& files
) {
    CURL* curl = curl_easy_init();
    if(!curl)
        return false;

    struct curl_slist* recipients = NULL;
    upload_context upload_ctx{0, ""};

    curl_easy_setopt(curl, CURLOPT_URL, smtp_server.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
    curl_easy_setopt(curl, CURLOPT_USERNAME, email.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, email.c_str());
    
    recipients = curl_slist_append(recipients, recipient.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    if(files.empty()) {
        upload_ctx.payload = 
            "From: " + email + "\r\n"
            "To: " + recipient + "\r\n"
            "Subject: " + subject + "\r\n"
            "Content-Type: " +
                (is_html ? "text/html" : "text/plain") +
            "; charset=UTF-8\r\n\r\n" +
            message + "\r\n";

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    }
    else setup_mime_structure(
        curl,
        email,
        recipient,
        subject,
        message,
        is_html,
        files
    );

    CURLcode res = curl_easy_perform(curl);
    bool success = (res == CURLE_OK);

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);

    return success;
}

bool quoneq_smtp_client::send_mail(
    const std::string& smtp_server,
    const std::string& email,
    const std::string& password,
    const std::string& recipient,
    const std::string& subject,
    const std::string& message,
    const std::vector<std::string>& files
) {
    return quoneq_smtp_client::send_email(
        smtp_server,
        email,
        password,
        recipient,
        subject,
        message,
        false,
        files
    );
}

bool quoneq_smtp_client::send_mail_html(
    const std::string& smtp_server,
    const std::string& email,
    const std::string& password,
    const std::string& recipient,
    const std::string& subject,
    const std::string& html_message,
    const std::vector<std::string>& files
) {
    return quoneq_smtp_client::send_email(
        smtp_server,
        email,
        password,
        recipient,
        subject,
        html_message,
        true,
        files
    );
}
