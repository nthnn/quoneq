#include <iostream>

// Include the Quoneq FTP and network utility headers.
#include <quoneq/ftp.hpp>
#include <quoneq/net.hpp>

// Define the base FTP URL for the FTP server.
// In this example, the FTP server is located at 192.168.100.122.
#define FTP_URL "ftp://192.168.100.122"

// Forward declaration of the net_cleanup function.
void net_cleanup();

int main() {
    // Print a message indicating that network initialization is starting.
    std::cout << "Initializing Quoneq..." << std::endl;

    // Initialize network resources required by Quoneq.
    // This might initialize any underlying libraries (like libcurl) or perform setup tasks.
    quoneq_net::init();

    // Notify the user that the program will now attempt to upload a file to the FTP server.
    std::cout << "Uploading file to: "
        << FTP_URL << std::endl;

    // Attempt to upload the local file "README.md" to the FTP server.
    // The target path is formed by appending "/README.md" to the base FTP URL.
    auto response = quoneq_ftp_client::upload(
        FTP_URL + std::string("/README.md"),
        "README.md"
    );

    // Check if the upload was successful by verifying the response code.
    // A response code of 200 indicates success.
    if(response->responseCode != 200) {
        // If the upload failed, print the error message from the response.
        std::cout << "Error Message:" <<
            std::endl << response->errorMessage <<
            std::endl;

        // Clean up network resources before exiting.
        net_cleanup();
        return 0;
    }

    // If the upload was successful, inform the user.
    std::cout << "File succesfully uploaded!" << std::endl;

    // Attempt to read the file "README.md" back from the FTP server.
    auto readResponse = quoneq_ftp_client::read(
        FTP_URL + std::string("/README.md")
    );

    // Check if reading the file was successful (response code 200).
    if(readResponse->responseCode != 200) {
        // If the read operation failed, print the error message.
        std::cout << "Error Message:" <<
            std::endl << readResponse->errorMessage <<
            std::endl;

        // Clean up network resources before exiting.
        net_cleanup();
        return 0;
    }

    // If the file was read successfully, print its contents.
    std::cout << "File content:"
        << std::endl << readResponse->content
        << std::endl;

    // Clean up network resources before the program exits.
    net_cleanup();
    return 0;
}

// This function calls quoneq_net::cleanup() to release any allocated network resources,
// and then prints a confirmation message to the console.
void net_cleanup() {
    quoneq_net::cleanup();
    std::cout << "Cleaned up Quoneq network." << std::endl;
}
