// server.cpp
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <thread>

using namespace std;

int BUFFER_SIZE = 4096;
const int SERVER_PORT = 5555;  // Server port

// Function to handle each client connection
void* handle_client(int* client_sock_ptr) {
    int client_sock = *client_sock_ptr;
    delete client_sock_ptr;  // Free the allocated memory

    char buffer[BUFFER_SIZE];
    int read_bytes;

    // Read data from client until the connection is closed
    while ((read_bytes = recv(client_sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[read_bytes] = '\0';                           // Null-terminate the received string
        cout << "Received from client: " << buffer << endl;  // Log the received message

        // Check the message content
        string received_message(buffer);
        if (received_message.find("User") != string::npos && received_message.find("watched video") != string::npos) {
            cout << "A logged-in user watched a video!" << endl;
        } else if (received_message.find("Video") != string::npos && received_message.find("viewed") != string::npos) {
            cout << "An anonymous user watched a video!" << endl;
        } else if (received_message.find("Video liked by user") != string::npos) {
            cout << "A video was liked by a user!" << endl;
        }

        // Send acknowledgment back to the client
        const char* ack_message = "Message received\n";
        send(client_sock, ack_message, strlen(ack_message), 0);  // Send response
        cout << "Acknowledgment sent to client." << endl;
    }

    if (read_bytes == 0) {
        cout << "Connection closed by client." << endl;
    } else if (read_bytes < 0) {
        perror("Error receiving data");
    }

    close(client_sock);  // Close the client socket
    cout << "Client connection closed." << endl;
    // pthread_exit(NULL);
    return 0;
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);  // Create a socket. AF_INET means IPv4 and SOCK_STREAM is TCP
    if (sock < 0) {
        perror("Error creating socket");
        return -1;
    }

    // Set SO_REUSEADDR to allow reuse of local addresses
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Error setting SO_REUSEADDR");
        close(sock);
        return -1;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any address
    sin.sin_port = htons(SERVER_PORT);

    if (bind(sock, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
        perror("Error binding socket");
        close(sock);
        return -1;
    }

    if (listen(sock, 5) < 0) {
        perror("Error listening to the socket");
        close(sock);
        return -1;
    }

    cout << "Server is running and waiting for client connections on port " << SERVER_PORT << "..." << endl;

    while (true) {  // Loop to accept multiple connections
        struct sockaddr_in client_sin;
        socklen_t addr_len = sizeof(client_sin);

        // Accept client connection
        int* client_sock = new int;
        *client_sock = accept(sock, (struct sockaddr*)&client_sin, &addr_len);
        if (*client_sock <= 0) {
            perror("Error accepting client connection");
            delete client_sock;
            continue;  // Continue to accept the next connection
        }

        cout << "Client connected!" << endl;  // Debug: Confirm client connection

        // Create a thread to handle the client
        thread client_thread;
        try {
            client_thread = thread(&handle_client, client_sock);
        } catch (const std::system_error& e) {
            perror("Error creating thread");
            close(*client_sock);
            delete client_sock;
            continue;
        }

        // Detach the thread so that it cleans up after itself
        client_thread.detach();
    }

    close(sock);  // Close the server socket
    return 0;
}
