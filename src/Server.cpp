#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <algorithm>

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();
    void run();

private:
    int listenSocket;
    std::string password;
    std::vector<int> clients;
    void processInstruction(int clientSocket);
};

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
    // Close the listening socket and all client sockets.
    close(listenSocket);
    // Close all client sockets.
	for (std::vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
		close(*it);
	}
}

void Server::processInstruction(int clientSocket) {
    // Read and process instructions from the connected client.
    char buffer[256];
    int bytesRead = read(clientSocket, buffer, sizeof(buffer));

    if (bytesRead <= 0) {
        // Handle disconnection or error
        close(clientSocket);
        std::cout << "Client " << clientSocket << " disconnected." << std::endl;

        // Remove the client socket from the vector
	// Find the clientSocket in the vector and erase it.
	std::vector<int>::iterator it = std::find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	} else {
        // Assuming the received data is a null-terminated string.
        buffer[bytesRead] = '\0';

        // Process the instruction (replace this with your own logic)
        std::cout << "Received instruction from client " << clientSocket << ": " << buffer << std::endl;
    	}
	}
}

void Server::run() {
    // Create a structure to hold information about the listening socket and all client sockets.
    std::vector<pollfd> fds;
    pollfd listenFd;
    listenFd.fd = listenSocket;
    listenFd.events = POLLIN;
    fds.push_back(listenFd);

    while (true) {
        // Use poll to wait for activity on any of the sockets.
        int numReady = poll(fds.data(), fds.size(), -1);

        if (numReady < 0) {
            throw std::runtime_error("Error in poll");
        }

        // Check if the listening socket is ready for a new connection.
        if (fds[0].revents & POLLIN) {
            sockaddr_in clientAddress;
            socklen_t clientAddressSize = sizeof(clientAddress);
            int clientSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressSize);
            if (clientSocket == -1) {
                throw std::runtime_error("Failed to accept connection.");
            }

            // Add the new client socket to the vector.
            pollfd newClientFd;
            newClientFd.fd = clientSocket;
            newClientFd.events = POLLIN;
            fds.push_back(newClientFd);

            clients.push_back(clientSocket);

            std::cout << "Client " << clientSocket << " connected." << std::endl;
        }

        // Check all connected clients for data.
		for (size_t i = 1; i < fds.size(); ++i) {
			if (fds[i].revents & POLLIN) {
				// Find the corresponding client socket in the vector
				int clientSocket = fds[i].fd;

				// Process instruction for the client
				processInstruction(clientSocket);
			}
		}
    }
}
