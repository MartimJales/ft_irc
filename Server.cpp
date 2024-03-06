#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

// Function to convert a string to camel case
std::string toCase(const std::string& input) {
	std::string result;
	bool capitalizeNext = false;

	for (char ch : input)
		if (std::isalpha(ch))
			result += std::toupper(ch);
	return result;
}

int main() {
	// Create a socket
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		perror("Error creating server socket");
		return EXIT_FAILURE;
	}

	// Set up the server address structure
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(8082); // You can change the port as needed

	// Bind the socket to the specified address and port
	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		perror("Error binding server socket");
		close(serverSocket);
		return EXIT_FAILURE;
	}

	// Listen for incoming connections
	if (listen(serverSocket, 5) == -1) {
		perror("Error listening for connections");
		close(serverSocket);
		return EXIT_FAILURE;
	}

	std::cout << "Server is listening for incoming connections..." << std::endl;

	// Accept a client connection
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket == -1) {
		perror("Error accepting client connection");
		close(serverSocket);
		return EXIT_FAILURE;
	}

	std::cout << "Client connected!" << std::endl;
	while (true) {
		// Receive data from the client
		char buffer[1024];
		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRead == -1) {
			perror("Error receiving data from the client");
		} else if (bytesRead == 0) {
			std::cout << "Client disconnected." << std::endl;
		} else {
			buffer[bytesRead] = '\0'; // Null-terminate the received data
			std::cout << "Received from client: " << buffer << std::endl;

			// Convert the message to camel case
			std::string camelCaseMessage = toCase(buffer);

			// Send the camel case message back to the client
			send(clientSocket, camelCaseMessage.c_str(), camelCaseMessage.length(), 0);

			// Check if the client wants to disconnect
			if (strcmp(buffer, "EXIT") == 0) {
				std::cout << "Client requested disconnect. Disconnecting..." << std::endl;
				close(clientSocket);
				break; // Exit the loop and close the server
			}
		}
	}

	// Close the server socket
	close(clientSocket);
	close(serverSocket);

	return EXIT_SUCCESS;
}
