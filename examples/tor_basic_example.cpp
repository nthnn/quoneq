#include <iostream>

// Include the Quoneq network and Tor-enabled HTTP client headers.
// These provide the functionality to initialize network resources and send HTTP requests via Tor.
#include <quoneq/net.hpp>
#include <quoneq/tor.hpp>

// Define the .onion address for the target site (OnionTalk in this example).
#define ONIONTALK_ONION "http://n44z5vtkhti5qncg3hswwykir2vy7kifptnykxi4nbso4iya3gtiufid.onion/"

// Forward declaration of the network cleanup function.
void net_cleanup();

int main() {
    // Inform the user that the network system is initializing.
    std::cout << "Initializing Quoneq..." << std::endl;
    
    // Initialize network resources needed by the Quoneq library.
    quoneq_net::init();

    // Check if the Tor network is running by using the Tor client utility.
    if(!quoneq_tor_client::is_tor_running()) {
        // If Tor is not running, print an error message.
        std::cout << "TOR isn't running!" << std::endl;

        // Clean up any network resources that were allocated.
        net_cleanup();

        // Exit the program since Tor is required for further operations.
        return 0;
    }

    // Display the onion URL to which the HTTP GET request will be sent.
    std::cout << "Sending request to: " << ONIONTALK_ONION << std::endl;

    // Send an HTTP GET request over the Tor network to the specified onion URL.
    // The response (including status, content, and error messages, if any) is stored in 'response'.
    auto response = quoneq_tor_client::get(ONIONTALK_ONION);

    // Output the HTTP response status code.
    std::cout << "Response status: " << response->status << std::endl;

    // Check if the response status code indicates an error (i.e., not 200 OK).
    if(response->status != 200)
        std::cout << "Error Message:" <<
            std::endl << response->errorMessage <<
            std::endl;
    else {
        std::cout << "Content:" << std::endl;
        std::cout << response->content << std::endl;
    }

    // Clean up network resources before program termination.
    net_cleanup();
    return 0;
}

// This function calls quoneq_net::cleanup() to properly release any network resources,
// and prints a confirmation message.
void net_cleanup() {
    quoneq_net::cleanup();
    std::cout << "Cleaned up Quoneq network." << std::endl;
}
