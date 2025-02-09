#include <iostream>

// Include the Quoneq network and SMTP client libraries.
#include <quoneq/net.hpp>
#include <quoneq/smtp.hpp>

// Define constants for the SMTP server settings, sender credentials, and email details.
#define SMTP_GATEWAY    "smtp://smtp.example.com:587"   // The SMTP server address and port.
#define SMTP_SENDER     "admin@example.com"             // The sender's email address.
#define SMTP_PASSKEY    "password"                      // The sender's email password.

#define EMAIL_RECIPIENT "user@example.com"              // The recipient's email address.
#define EMAIL_SUBJECT   "Test HTML Email"               // The subject line for the email.
#define EMAIL_CONTENT   "<h1>Hello, world!</h1>"         // The HTML content of the email.

// Forward declaration of the network cleanup function.
void net_cleanup();

int main() {
    // Print a message indicating that network resources are being initialized.
    std::cout << "Initializing Quoneq..." << std::endl;
    // Initialize the Quoneq network (e.g., set up libcurl, etc.).
    quoneq_net::init();

    // Inform the user that an email is about to be sent.
    std::cout << "Emailing " << EMAIL_RECIPIENT
        << "..." << std::endl;

    // Send an HTML email using the Quoneq SMTP client.
    // The send_mail_html method sends an email via the provided SMTP gateway, using the defined sender credentials.
    // It sends the email to EMAIL_RECIPIENT with the subject and HTML content provided.
    // An attachment "image.png" is included in the email.
    bool success = quoneq_smtp_client::send_mail_html(
        SMTP_GATEWAY,
        SMTP_SENDER, 
        SMTP_PASSKEY, 
        EMAIL_RECIPIENT, 
        EMAIL_SUBJECT, 
        EMAIL_CONTENT,
        // List of attachments (in this case, a single file "image.png").
        {"image.png"}
    );

    // Print the outcome of the email sending operation.
    std::cout << (success
        ? "Email sent successfully!"
        : "Email sending failed!"
    ) << std::endl;

    // Clean up network resources.
    net_cleanup();
    return 0;
}

// This function calls quoneq_net::cleanup() to release any network-related resources
// (such as those allocated by libcurl) and prints a confirmation message.
void net_cleanup() {
    quoneq_net::cleanup();
    std::cout << "Cleaned up Quoneq network." << std::endl;
}
