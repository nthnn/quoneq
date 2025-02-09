#include <iostream>

#include <quoneq/http.hpp>        // For performing HTTP operations via the Quoneq HTTP client
#include <quoneq/net.hpp>         // For initializing and cleaning up network resources

// Define a constant for the Cat Fact API URL
#define CAT_FACT "https://catfact.ninja/fact"

// Forward declaration for the network cleanup function
void net_cleanup();

int main() {
    // Inform the user that the network subsystem is being initialized
    std::cout << "Initializing Quoneq..." << std::endl;

    // Initialize the network resources (e.g., set up any necessary libraries or configurations)
    quoneq_net::init();

    // Print the target URL for the HTTP GET request
    std::cout << "Sending request to: " << CAT_FACT << std::endl;

    // Send an HTTP GET request to the Cat Fact API
    // The quoneq_http_client::get function returns a unique pointer to a response object
    auto response = quoneq_http_client::get(CAT_FACT);

    // Output the HTTP status code received from the server
    std::cout << "Response status: " << response->status << std::endl;

    // If the response status is not 200 (OK), print the error message returned by the client
    if(response->status != 200)
        std::cout << "Error Message:" <<
            std::endl << response->errorMessage <<
            std::endl;
    else {
        // Otherwise, print the content of the response (e.g., the cat fact in JSON format)
        std::cout << "Content:" << std::endl;
        std::cout << response->content << std::endl;
    }

    // Clean up network resources (release any allocated resources, etc.)
    net_cleanup();
    return 0;
}

// This function calls quoneq_net::cleanup() to release any resources or settings
// that were initialized by quoneq_net::init() earlier.
void net_cleanup() {
    quoneq_net::cleanup();
    std::cout << "Cleaned up Quoneq network." << std::endl;
}
