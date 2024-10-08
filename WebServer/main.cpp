#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;

int main() {
    const int server_port = 5555;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creating socket");
        return -1;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port);

    if (bind(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("Error binding socket");
        return -1;
    }

    if (listen(sock, 5) < 0) {
        perror("Error listening to the socket");
        return -1;
    }

    cout << "Waiting for client connection..." << endl;

    struct sockaddr_in client_sin;
    unsigned int addr_len = sizeof(client_sin);
    int client_sock = accept(sock, (struct sockaddr *) &client_sin, &addr_len);
    if (client_sock < 0) {
        perror("Error accepting client");
        return -1;
    }

    char buffer[4096];
    int read_bytes = recv(client_sock, buffer, sizeof(buffer), 0);
    if (read_bytes > 0) {
        buffer[read_bytes] = '\0';  // Null-terminate the received string
        cout << "Received from client: " << buffer << endl;

        // Check if the message is about a video being liked
        string received_message(buffer);
        if (received_message.find("Video liked by user") != string::npos) {
            cout << "A video was liked by a user!" << endl;
        }

        // Send acknowledgment back to Node.js
        const char *ack_message = "Message received\n";
        send(client_sock, ack_message, strlen(ack_message), 0);
    } else {
        cout << "No data received or error in recv()" << endl;
    }

    close(client_sock);  // Close the client socket
    close(sock);         // Close the server socket
    return 0;
}
