#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

int main() {
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating client socket");
        return EXIT_FAILURE;
    }

    // Set up the server address structure
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8082); // Match the server port
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        perror("Invalid server address");
        close(clientSocket);
        return EXIT_FAILURE;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to the server");
        close(clientSocket);
        return EXIT_FAILURE;
    }

    std::cout << "Connected to the server!" << std::endl;

    while (true) {
        // Get user input
        std::cout << "Enter a message to send to the server ('EXIT' to exit): ";
        std::string userMessage;
        std::getline(std::cin, userMessage);

        // Send the user input to the server
        send(clientSocket, userMessage.c_str(), userMessage.length(), 0);

        // Check if the user wants to exit
        if (userMessage == "EXIT") {
            std::cout << "Exiting..." << std::endl;
            break;
        }

        // Receive and display the response from the server
        char buffer[1024];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            perror("Error receiving data from the server");
            break;
        }
        buffer[bytesRead] = '\0'; // Null-terminate the received data
        std::cout << "Received from server: " << buffer << std::endl;
    }

    // Close the socket
    close(clientSocket);

    return EXIT_SUCCESS;
}
