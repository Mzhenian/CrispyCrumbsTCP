// server.cpp
#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <sstream>

using namespace std;

// Data structures
unordered_map<string, unordered_set<string>> userVideosMap;  // userID -> set of videoIDs
unordered_map<string, unordered_set<string>> videoRecommendationsMap;  // videoID -> set of recommended videoIDs

mutex dataMutex;  // Mutex for thread-safe access to data structures

// Function to handle communication with a client
void handle_client(int client_sock) {
    char buffer[4096];
    int read_bytes;

    // Keep the connection open until the client closes it
    while (true) {
        // Receive data from the client
        read_bytes = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (read_bytes > 0) {
            buffer[read_bytes] = '\0';  // Null-terminate the received string
            string received_message(buffer);

            cout << "Received from client: " << received_message << endl;

            // Process the message
            if (received_message.find("WATCH") == 0) {
                // WATCH <userId> <videoId>
                istringstream iss(received_message);
                string command, userId, videoId;
                iss >> command >> userId >> videoId;

                if (userId != "ANONYMOUS") {
                    // Update user-videos mapping
                    lock_guard<mutex> lock(dataMutex);
                    userVideosMap[userId].insert(videoId);

                    // Update video-recommendations mapping
                    for (const auto& otherVideoId : userVideosMap[userId]) {
                        if (otherVideoId != videoId) {
                            videoRecommendationsMap[videoId].insert(otherVideoId);
                            videoRecommendationsMap[otherVideoId].insert(videoId);
                        }
                    }
                }
            }
            else if (received_message.find("GET_RECOMMENDATIONS") == 0) {
                // GET_RECOMMENDATIONS <videoId>
                istringstream iss(received_message);
                string command, videoId;
                iss >> command >> videoId;

                unordered_set<string> recommendations;
                {
                    lock_guard<mutex> lock(dataMutex);
                    recommendations = videoRecommendationsMap[videoId];
                }

                // Build the response message
                string response = "RECOMMENDATIONS";
                for (const auto& vid : recommendations) {
                    response += " " + vid;
                }
                response += "\n";

                // Send the response
                send(client_sock, response.c_str(), response.length(), 0);
            }
            else {
                // Unknown command
                string error_message = "ERROR Unknown command\n";
                send(client_sock, error_message.c_str(), error_message.length(), 0);
            }

        } else if (read_bytes == 0) {
            cout << "Connection closed by client." << endl;
            break;  // Exit the loop if the client closes the connection
        } else {
            perror("Error receiving data");
            break;  // Exit the loop on error
        }
    }

    close(client_sock);  // Close the client socket
    cout << "Client connection closed." << endl;
}

int main() {
    const int server_port = 5555;  // Server port
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);  // Create a socket
    if (server_sock < 0) {
        perror("Error creating socket");
        return -1;
    }

    // Set SO_REUSEADDR to allow reuse of local addresses
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Error setting SO_REUSEADDR");
        close(server_sock);
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any address
    server_addr.sin_port = htons(server_port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        close(server_sock);
        return -1;
    }

    if (listen(server_sock, 5) < 0) {
        perror("Error listening on socket");
        close(server_sock);
        return -1;
    }

    cout << "Server is running and waiting for client connections on port " << server_port << "..." << endl;

    while (true) {  // Loop to accept multiple connections
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        // Accept client connection
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0) {
            perror("Error accepting client connection");
            continue;  // Continue to accept the next connection
        }

        cout << "Client connected!" << endl;  // Debug: Confirm client connection

        // Create a new thread to handle the client
        thread client_thread(handle_client, client_sock);

        // Detach the thread so it can run independently
        client_thread.detach();
    }

    close(server_sock);  // Close the server socket
    return 0;
}
