#include "Server.hpp"

Server::Server(int port, const std::string& password) : listenSocket(-1), password(password) {
	// Create a socket for listening.
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == -1) {
		throw std::runtime_error("Failed to create socket.");
	}

	// Bind the socket to the port.
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
		throw std::runtime_error("Failed to bind socket.");
	}

	// Start listening for incoming connections.
	if (listen(listenSocket, 10) == -1) {
		throw std::runtime_error("Failed to listen on socket.");
	}
}

Server::~Server() {
	// Close the listening socket.
	close(listenSocket);
}

void Server::run() {
	// Accept incoming connections and create a new client for each one.
	while (true) {
		sockaddr_in clientAddress;
		socklen_t clientAddressSize = sizeof(clientAddress);
		int clientSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);
		if (clientSocket == -1) {
			throw std::runtime_error("Failed to accept connection.");
		}

		// Create a new client and add it to the list of clients.
		clients.push_back(Client(clientSocket));
	}
}