// server.cpp
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "../jsonConverter.hpp"
#include "../recommendationEngine.hpp"

using namespace std;
namespace JC = JsonConverter;

int BUFFER_SIZE = 4096;
const int SERVER_PORT = 5555;

void send_error_message(int client_sock, const string& error_message) {
    string response = "{\"error\": \"" + error_message + "\"}\n";
    send(client_sock, response.c_str(), response.length(), 0);
}

// handle each client connection
void* handle_client(int* client_sock_ptr) {
    RecommendationEngine rcmndEngine;
    int client_sock = *client_sock_ptr;
    delete client_sock_ptr;  // Free the allocated memory

    char buffer[BUFFER_SIZE];
    int read_bytes;
    string message;
    size_t message_end;
    bool newMessage;

    // Read data from client until the connection is closed
    while ((newMessage = message.find('\n') == string::npos) && (read_bytes = recv(client_sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        if (newMessage) {
            buffer[read_bytes] = '\0';  // Null-terminate the received string
            message += buffer;          // Accumulate the received data
        }
        message_end = message.find('\n');
        if (message_end == string::npos) {
            cout << "Received just a buffered part from client..." << endl;
            continue;
        }

        cout << "Received from client: " << message << endl;  // Log the received message

        JC::jsonObject jsonObj;
        string action;

        // Parse the JSON message
        try {
            auto it = message.begin();
            jsonObj = *JC::parseJsonObject(message, it);
        } catch (const invalid_argument& e) {
            perror("Error parsing JSON.");
            send_error_message(client_sock, "Error parsing JSON.");
            message.erase(0, message_end + 1);
            while (!message.empty() && (message.starts_with('\n') || message.starts_with('\0'))) {
                message.erase(0, 1);
            }
            continue;
        }
        try {
            action = get<string>(jsonObj.at("action").value);
        } catch (const exception& e) {
            cerr << "Missing 'action' string value in JSON object" << endl;
            send_error_message(client_sock, "Missing 'action' string value in JSON object.");
            message.erase(0, message_end + 1);
            while (!message.empty() && (message.starts_with('\n') || message.starts_with('\0'))) {
                message.erase(0, 1);
            }
            continue;
        }

        if (action == "viewed") {
            string videoId;
            int views;

            try {
                videoId = get<string>(jsonObj.at("videoId").value);
                views = get<int>(jsonObj.at("views").value);
            } catch (const exception& e) {
                cerr << "action == viewed: Missing 'videoId' or 'views' value in JSON object" << endl;
                send_error_message(client_sock, "Missing 'videoId' or 'views' value in JSON object.");
                message.erase(0, message_end + 1);
                while (!message.empty() && (message.starts_with('\n') || message.starts_with('\0'))) {
                    message.erase(0, 1);
                }
                continue;
            }

            rcmndEngine.loadVideo(videoId, views);

            cout << "Video " << videoId << " with " << views << " views updated to the recommendation engine." << endl;

        } else if (action == "watching") {
            vector<string> userWatchHistory;
            string userId;

            try {
                userWatchHistory = get<vector<string>>(jsonObj.at("watchHistory").value);
                userId = get<string>(jsonObj.at("userId").value);
            } catch (const exception& e) {
                cerr << "action == watching: Missing 'userId' or 'watchHistory' value in JSON object" << endl;
                send_error_message(client_sock, "Missing 'userId' or 'watchHistory' value in JSON object.");

                message.erase(0, message_end + 1);
                while (!message.empty() && (message.starts_with('\n') || message.starts_with('\0'))) {
                    message.erase(0, 1);
                }
                continue;
            }

            rcmndEngine.loadUser(userWatchHistory, userId);

            cout << "loading user watch history" << endl;
        } else if (action == "get recommendations") {
            vector<string> userWatchHistory;
            string videoId;

            try {
                if (jsonObj.contains("watchHistory")) {
                    userWatchHistory = get<vector<string>>(jsonObj.at("watchHistory").value);
                } else {
                    userWatchHistory = {};
                }
                videoId = get<string>(jsonObj.at("videoId").value);
            } catch (const exception& e) {
                cerr << "action == watching: Missing 'videoId' or 'watchHistory' value in JSON object" << endl;
                send_error_message(client_sock, "Missing 'videoId' or 'watchHistory' value in JSON object.");

                message.erase(0, message_end + 1);
                while (!message.empty() && (message.starts_with('\n') || message.starts_with('\0'))) {
                    message.erase(0, 1);
                }
                continue;
            }

            vector<string> recommendations = rcmndEngine.getRecommendations(userWatchHistory, videoId);

            JC::jsonObject recommendationsObj;
            recommendationsObj.insert(make_pair("recommendedVideosList", &recommendations));
            // recommendationsObj["recommendedVideosList"] = JC::jsonValue(make_unique<vector<string>>(recommendations));
            string response = JC::serialize(recommendationsObj);

            // Send the recommendations back to the client
            send(client_sock, response.c_str(), response.length(), 0);  // Send response
            cout << "sent to client: " << response << endl;
        } else {
            cerr << "requested unknown action" << endl;
            message.erase(0, message_end + 1);
            while (!message.empty() && (message.starts_with('\n') || message.starts_with('\0'))) {
                message.erase(0, 1);
            }
            continue;
        }

        message.erase(0, message_end + 1);
        while (!message.empty() && (message.starts_with('\n') || message.starts_with('\0'))) {
            message.erase(0, 1);
        }
    }

    if (read_bytes == 0) {
        cout << "Connection closed by client." << endl;
    } else if (read_bytes < 0 || !message.empty()) {
        perror("Error receiving data, closing connection.");
    }

    close(client_sock);  // Close the client socket
    cout << "Client connection closed." << endl;
    return 0;
}

// open a socket to listen for incoming connections and route them to the handle_client function
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

    // Loop to accept multiple connections
    while (true) {
        struct sockaddr_in client_sin;
        socklen_t addr_len = sizeof(client_sin);

        // Accept client connection
        int* client_sock = new int;
        *client_sock = accept(sock, (struct sockaddr*)&client_sin, &addr_len);
        if (*client_sock <= 0) {
            perror("Error accepting client connection");
            delete client_sock;
            continue;  // Continue to accept other connections
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

    close(sock);  // Close the server socket, right after the infinite loop...
    return 0;
}
