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
    // Close the listening socket and all client sockets.
    close(listenSocket);
    // Close all client sockets.
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		close(it->getClientSocket());
	}
}

void Server::processInstruction(int clientSocket, int index)
{
	// Read and process instructions from the connected client.
    char buffer[256];
	bzero(buffer, sizeof(buffer));
	int bytesRead = read(clientSocket, buffer, sizeof(buffer));
	// If the client disconnected, remove the client from the server.
	// IMPORTANT
	if (bytesRead <= 0)
	{
		removeClient(clientSocket, index);
		return;
	}
	// Show the received message and the client socket
	executeCommand(buffer, index);
	std::cout << "{" << clientSocket << "}" << buffer << std::endl;
}

void Server::run() {
	// Create a structure to hold information about the listening socket and all client sockets.
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
			addClient();
		}

		// Check all connected clients for data.
		for (size_t i = 1; i < fds.size(); ++i)
		{
			if (fds[i].revents & POLLIN)
			{
				// Process the instruction from the client.
				processInstruction(fds[i].fd, i);
				// Send a welcome message to the client.
				//write(fds[i].fd, ":server 421 teste :teste", 22);
			}
		}
	}
}

void Server::addClient(void)
{
	sockaddr_in clientAddress;
	socklen_t clientAddressSize = sizeof(clientAddress);
	int clientSocket = accept(listenSocket, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressSize);
	if (clientSocket == -1)
	{
		throw std::runtime_error("Failed to accept connection.");
	}

	// if (checkPassword(clientSocket))
	// {
		Client newClient(clientSocket);
		// Add the new client socket to the vector.
		pollfd newClientFd;
		newClientFd.fd = clientSocket;
		newClientFd.events = POLLIN;
		fds.push_back(newClientFd);
		clients.push_back(newClient);
		std::cout << "Client " << clientSocket << " connected." << std::endl;
	// }
	// Send an hello world to the new client
	write(clientSocket, "SERVER: Hello World", 20);

}

void Server::removeClient(int clientSocket, int index)
{
	// Disconnect the client
	std::cout << "Client " << clientSocket << " disconnected." << std::endl;
	// Remove the client socket from the vector
	clients.erase(clients.begin() + index - 1);
	// Remove the client socket from the fds vector
	fds.erase(fds.begin() + index);
	// Close the client socket
	close(clientSocket);
}

bool Server::checkPassword(int clientSocket)
{
	// Check if the password is correct
	char buffer[256];
	bzero(buffer, sizeof(buffer));
	int bytesRead = read(clientSocket, buffer, sizeof(buffer));
	if (bytesRead < 0)
	{
		throw std::runtime_error("Error reading from socket");
	}
	// Print the received password
	std::cout << "Received password: " << buffer << std::endl;
	// The password can have a newline in the end, so we remove it
	if (strncmp(buffer, password.c_str(), password.size()) != 0)
	{
		std::cout << "Client " << clientSocket << " sent an incorrect password." << buffer << std::endl;
		int n = write(clientSocket, "Incorrect password", 18);
		if (n < 0)
		{
			throw std::runtime_error("Error writing to socket");
		}
		close(clientSocket);
		return false;
	}
	int n = write(clientSocket, "Correct password", 17);
	if (n < 0)
	{
		throw std::runtime_error("Error writing to socket");
	}
	return true;
}

void Server::executeCommand(const std::string& command, int indexClient){
	std::cout << "Command: " << command << std::endl;
	(void)indexClient;
}
