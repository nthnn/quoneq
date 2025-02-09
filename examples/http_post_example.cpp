#include <iostream>

// Include the Quoneq HTTP client and network utilities.
#include <quoneq/http.hpp>
#include <quoneq/net.hpp>

// Define the target URL. In this case, it is set to a public API that returns cat facts.
#define LOCALHOST "https://catfact.ninja/fact"

// Forward declaration for the network cleanup function.
void net_cleanup();

int main() {
    // Print a message to indicate the start of network initialization.
    std::cout << "Initializing Quoneq..." << std::endl;

    // Initialize the Quoneq network resources (this may setup libcurl or other networking libraries).
    quoneq_net::init();

    // Inform the user that an HTTP request is being sent to the specified URL.
    std::cout << "Sending request to: " << LOCALHOST << std::endl;

    // Perform an HTTP POST request using the Quoneq HTTP client.
    // - The target URL is LOCALHOST.
    // - Empty maps are passed for form data, headers, and cookies.
    // - A file is specified as an attachment with the key "fileToUpload" and its corresponding local file path.
    auto response = quoneq_http_client::post(
        LOCALHOST,
        {},   // No form fields
        {},   // No custom headers
        {},   // No cookies
        {{"fileToUpload", "~/document.pdf"}}
    );

    // Output the HTTP response status code received from the server.
    std::cout << "Response status: " << response->status << std::endl;

    // Check if the response status code is not 200 (OK).
    if(response->status != 200)
        std::cout << "Error Message:" <<
            std::endl << response->errorMessage <<
            std::endl;
    else {
        std::cout << "Content:" << std::endl;
        std::cout << response->content << std::endl;
    }

    // Clean up the network resources.
    net_cleanup();
    return 0;
}

// This function calls the network cleanup routine provided by quoneq_net, which
// is used to release any resources allocated during initialization.
void net_cleanup() {
    quoneq_net::cleanup();
    std::cout << "Cleaned up Quoneq network." << std::endl;
}
