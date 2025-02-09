#include <iostream>

// Include the Quoneq network utility header for network initialization and cleanup.
#include <quoneq/net.hpp>
// Include the Quoneq Telnet client header for performing Telnet operations.
#include <quoneq/telnet.hpp>

// Define the Telnet server URL as a macro.
// In this example, the Telnet server is assumed to be running locally on port 2323.
#define TELNET_URL  "telnet://localhost:2323"

// Forward declaration of the net_cleanup() function, which cleans up network resources.
void net_cleanup();

int main() {
    // Output a message indicating that the network (Quoneq) is being initialized.
    std::cout << "Initializing Quoneq..." << std::endl;

    // Initialize network resources. This might set up necessary libraries or configurations.
    quoneq_net::init();

    // Inform the user that a connection attempt to the Telnet server is being made.
    std::cout << "Connecting to Telnet server on localhost:2323...\n";

    // Attempt to connect to the Telnet server using the defined URL.
    // The connect() method returns a unique pointer to a quoneq_telnet_response.
    auto connect_response = quoneq_telnet_client::connect(
        TELNET_URL
    );

    // Check if the connection response contains an error message.
    if(!connect_response->error_message.empty()) {
        // If there is an error, output the error message to the standard error stream.
        std::cerr << "Connection error: "
            << connect_response->error_message << "\n";

        // Clean up network resources before exiting.
        net_cleanup();
        return 0;
    }
    else std::cout << "Server initial response:\n"
        << connect_response->content << "\n";

    // Output a message indicating that a Telnet command is about to be sent.
    std::cout << "\nSending command: 'echo Hello, World!'\n";

    // Send the Telnet command "echo Hello, World!" using the quote() method.
    // This method connects to the Telnet server, sends the command, and returns the response.
    auto command_response = quoneq_telnet_client::quote(
        TELNET_URL,
        "echo Hello, World!"
    );

    // Check if the command response contains an error message.
    if(!command_response->error_message.empty())
        std::cerr << "Command error: "
            << command_response->error_message << "\n";
    else std::cout << "Command output:\n"
        << command_response->content << "\n";

    // End the main function and return 0 indicating success.
    return 0;
}

// Definition of the net_cleanup() function.
// This function cleans up network resources by calling the cleanup method from the Quoneq network utility.
void net_cleanup() {
    quoneq_net::cleanup();
    std::cout << "Cleaned up Quoneq network." << std::endl;
}
