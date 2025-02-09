#include <iostream>

#include <quoneq/http.hpp>
#include <quoneq/net.hpp>

// Define the URL of the file to download (an image hosted on GitHub)
#define DOWNLOAD_URL "https://raw.githubusercontent.com/nthnn/nthnn/main/img_nthnn.png"

// Forward declaration for the network cleanup function.
void net_cleanup();

int main() {
    // Display an initialization message and set up the network resources.
    std::cout << "Initializing Quoneq..." << std::endl;
    quoneq_net::init();

    // Inform the user which file will be downloaded.
    std::cout << "Downloading image file from: " << DOWNLOAD_URL << std::endl;

    // Download the file from the specified URL and save it locally as "nthnn.png".
    // The download_file() function returns a response object containing the HTTP status code,
    // any error messages, and the response content.
    auto response = quoneq_http_client::download_file(
        DOWNLOAD_URL,
        "nthnn.png"
    );
    // Output the HTTP response status to the console.
    std::cout << "Response status: " << response->status << std::endl;

    // Check if the HTTP response status is not 200 (which means the request failed).
    if(response->status != 200)
        std::cout << "Error Message:" <<
            std::endl << response->errorMessage <<
            std::endl;
    else std::cout << "File successfully downloaded!" << std::endl;

    // Clean up network resources before exiting.
    net_cleanup();
    return 0;
}

// This function calls quoneq_net::cleanup() to release any resources
// allocated during network initialization, and then prints a message
// indicating that the cleanup is complete.
void net_cleanup() {
    quoneq_net::cleanup();
    std::cout << "Cleaned up Quoneq network." << std::endl;
}
