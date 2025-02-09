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
 * @file quoneq_smtp.hpp
 * @author [Nathanne Isip](https://github.com/nthnn)
 * @brief Provides an SMTP client for sending emails using libcurl.
 *
 * This header defines the quoneq_smtp_client class which supports sending emails via SMTP.
 * It allows sending plain text or HTML emails with optional file attachments.
 */
#ifndef QUONEQ_SMTP_HPP
#define QUONEQ_SMTP_HPP

#include <string>
#include <vector>

#include <curl/curl.h>

/**
 * @brief SMTP client class for sending emails.
 *
 * The quoneq_smtp_client class provides static methods to send emails using an SMTP server.
 * It supports sending emails with plain text or HTML content and allows attaching files.
 */
class quoneq_smtp_client {
private:
    /**
     * @brief Structure to track the upload state of the email payload.
     *
     * This structure is used by the payload_source callback function to keep track of the
     * number of bytes already read from the email payload.
     */
    struct upload_context {
        size_t bytes_read;       ///< Number of bytes already read.
        std::string payload;     ///< The complete email payload to be sent.
    };

    /**
     * @brief Callback function for reading the email payload.
     *
     * This function is called by libcurl when sending an email to provide chunks of data
     * from the email payload. It reads from an UploadContext.
     *
     * @param ptr Pointer to the buffer where data should be stored.
     * @param size Size of each data element.
     * @param nmemb Number of data elements.
     * @param userp Pointer to an UploadContext structure containing the payload.
     * @return The number of bytes read.
     */
    static size_t payload_source(
        void* ptr,
        size_t size,
        size_t nmemb,
        void* userp
    );

    /**
     * @brief Sets up the MIME structure for the email message.
     *
     * This function configures the MIME structure for the email including the sender, recipient,
     * subject, message body, and any file attachments. The MIME structure is used by libcurl to
     * construct the email.
     *
     * @param curl Pointer to the libcurl handle.
     * @param email The sender's email address.
     * @param recipient The recipient's email address.
     * @param subject The email subject.
     * @param message The email message body.
     * @param is_html Set to true if the message is HTML formatted.
     * @param files A vector of file paths for attachments.
     */
    static void setup_mime_structure(
        CURL* curl,
        const std::string& email,
        const std::string& recipient,
        const std::string& subject,
        const std::string& message,
        bool is_html,
        const std::vector<std::string>& files
    );

public:
    /**
     * @brief Sends an email via SMTP.
     *
     * This method sends an email to the specified recipient using the provided SMTP server.
     * It supports sending both plain text and HTML emails, with optional file attachments.
     *
     * @param smtp_server The address of the SMTP server.
     * @param email The sender's email address.
     * @param password The sender's email password.
     * @param recipient The recipient's email address.
     * @param subject The email subject.
     * @param message The email message body.
     * @param is_html True if the message body is HTML formatted, false if plain text.
     * @param files (Optional) A vector of file paths to attach to the email.
     * @return True if the email was sent successfully; false otherwise.
     */
    static bool send_email(
        const std::string& smtp_server,
        const std::string& email,
        const std::string& password,
        const std::string& recipient,
        const std::string& subject,
        const std::string& message,
        bool is_html,
        const std::vector<std::string>& files = {}
    );

    /**
     * @brief Sends a plain text email via SMTP.
     *
     * This method sends a plain text email using the specified SMTP server.
     *
     * @param smtp_server The address of the SMTP server.
     * @param email The sender's email address.
     * @param password The sender's email password.
     * @param recipient The recipient's email address.
     * @param subject The email subject.
     * @param message The plain text email message body.
     * @param files (Optional) A vector of file paths to attach to the email.
     * @return True if the email was sent successfully; false otherwise.
     */
    static bool send_mail(
        const std::string& smtp_server,
        const std::string& email,
        const std::string& password,
        const std::string& recipient,
        const std::string& subject,
        const std::string& message,
        const std::vector<std::string>& files = {}
    );

    /**
     * @brief Sends an HTML email via SMTP.
     *
     * This method sends an HTML formatted email using the specified SMTP server.
     *
     * @param smtp_server The address of the SMTP server.
     * @param email The sender's email address.
     * @param password The sender's email password.
     * @param recipient The recipient's email address.
     * @param subject The email subject.
     * @param html_message The HTML formatted email message body.
     * @param files (Optional) A vector of file paths to attach to the email.
     * @return True if the email was sent successfully; false otherwise.
     */
    static bool send_mail_html(
        const std::string& smtp_server,
        const std::string& email,
        const std::string& password,
        const std::string& recipient,
        const std::string& subject,
        const std::string& html_message,
        const std::vector<std::string>& files = {}
    );
};

#endif
