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
 * @file quoneq_net.hpp
 * @author [Nathanne Isip](https://github.com/nthnn)
 * @brief Provides network initialization and cleanup functions.
 *
 * This header defines the quoneq_net class, which encapsulates the
 * initialization and cleanup of network resources. It is intended to be
 * used before and after performing any network operations.
 */
#ifndef QUONEQ_NET_HPP
#define QUONEQ_NET_HPP

#include <string>

/**
 * @brief Network utility class.
 *
 * The quoneq_net class provides static methods for initializing and cleaning
 * up network resources. Call quoneq_net::init() before using any network
 * functions, and quoneq_net::cleanup() once you are finished.
 */
class quoneq_net {
private:
    static std::string cacert_path;

public:
    /**
     * @brief Initializes network resources.
     *
     * This function should be called before any network operations are performed.
     * It initializes any necessary network libraries or settings.
     */
    static void init();

    /**
     * @brief Cleans up network resources.
     *
     * This function should be called after all network operations are complete.
     * It releases any resources allocated during network initialization.
     */
    static void cleanup();

    /**
     * @brief Sets the CA certificate file path.
     *
     * This function allows specifying a custom CA certificate file used for 
     * secure SSL/TLS connections. This should be set before performing 
     * any network operations that require SSL verification.
     *
     * @param path The file path to the CA certificate.
     */
    static void set_ca_cert(std::string path);

    /**
     * @brief Retrieves the CA certificate file path.
     *
     * This function returns the CA certificate file path previously set by 
     * `set_ca_cert()`. If no path has been set, it may return an empty string.
     *
     * @return The CA certificate file path.
     */
    static std::string get_ca_cert();
};

#endif
