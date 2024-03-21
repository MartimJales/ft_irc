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
	executeCommand(buffer, index - 1);
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

	char* command_cstr = new char[command.size() + 1];
    std::strcpy(command_cstr, command.c_str());
	(void)indexClient;
	// Split the command to an array
	char *p = std::strtok(command_cstr, " \n");

	//List all users and nicknames
	if (command == "LIST\n")
		listClients();
	// Set the nickname of the client
	if (p != NULL && std::strcmp(p, "NICK") == 0)
	{
		p = std::strtok(NULL, " \n");
		changeUserNickName(p, indexClient);
	}
	// Set the username of the client
	if (p != NULL && std::strcmp(p, "USER") == 0)
	{
		p = std::strtok(NULL, " \n");
		changeUserName(p, indexClient);
	}
	// List all channels
	if (p != NULL && std::strcmp(p, "LIST_C") == 0)
	{
		listChannels();
	}
	// Join a channel
	if (p != NULL && std::strcmp(p, "JOIN") == 0)
	{
		p = std::strtok(NULL, " \n");
		joinChannel(p, indexClient);
	}
	// Send a direct message to a user
	if (p != NULL && std::strcmp(p, "PRIVMSG") == 0)
	{
		p = std::strtok(NULL, " \n");
		std::string nickname(p);
		p = std::strtok(NULL, "\n");
		std::string message(p);
		sendDirectMessage(nickname, message, indexClient);
	}
	// Send a message to a channel
	if (p != NULL && std::strcmp(p, "MSG_C") == 0)
	{
		p = std::strtok(NULL, " \n");
		std::string channel(p);
		p = std::strtok(NULL, "\n");
		std::string message(p);
		sendChannelMessage(channel, message, indexClient);
	}
	// List of every user in specific channel
	if (p != NULL && std::strcmp(p, "CHANNEL_USERS") == 0)
	{
		p = std::strtok(NULL, " \n");
		std::string channel(p);
		listChannelUsers(channel);
	}
	// Change or view the topic of a channel
	if (p != NULL && std::strcmp(p, "TOPIC") == 0)
	{
		p = std::strtok(NULL, " \n");
		std::string channel(p);
		p = std::strtok(NULL, "\n");
		// IF p is NUll, we are just viewing the topic
		if (p == NULL)
			viewChannelTopic(channel, indexClient);	
		else {
			std::string topic(p);
			changeChannelTopic(channel, topic, indexClient);
		}
	}
}

void Server::listClients() {
    std::cout << "List of clients:" << std::endl;
    std::vector<Client>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) {
        std::cout << "Nickname: " << it->getNickName() << std::endl;
		std::cout << "Username: " << it->getUsername() << std::endl;
    }
}

void Server::changeUserNickName(const std::string& nickname, int indexClient) {
	std::cout << "Client " << indexClient << " changed nickname to " << nickname << std::endl;
	clients[indexClient].setNickname(nickname);
}

void Server::changeUserName(const std::string& username, int indexClient) {
	std::cout << "Client " << indexClient << " changed username to " << username << std::endl;
	clients[indexClient].setUsername(username);
}

void Server::listChannels() {
	std::cout << "List of channels:" << std::endl;
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it) {
		std::cout << "Channel: " << it->getName() << std::endl;
	}
}

void Server::joinChannel(const std::string& channelName, int indexClient) {
	std::cout << "Client " << indexClient << " joined channel " << channelName << std::endl;
	// Check if the channel exists
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it) {
		if (it->getName() == channelName) {
			//TODO: Temos que retirar o cliente do canal anterior
			it->addMember(&clients[indexClient]);
			clients[indexClient].setCurrentChannel(channelName);
			break;
		}
	}
	// If the channel does not exist, create it
	if (it == channels.end()) {
		Channel newChannel(channelName);
		newChannel.addMember(&clients[indexClient]);
		newChannel.addOperator(&clients[indexClient]);
		channels.push_back(newChannel);
		clients[indexClient].setCurrentChannel(channelName);
	}
}

void Server::sendDirectMessage(const std::string& nickname, const std::string& message, int indexClient) {
	std::cout << clients[indexClient].getNickName() << " sent a direct message to " << nickname << ": " << message << std::endl;
	// Find the client with the nickname
	std::vector<Client>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it) {
		if (it->getNickName() == nickname) {
			// Send the message to the client
			write(it->getClientSocket(), message.c_str(), message.size());
			break;
		}
	}
}

// TODO: Temos que mudar isto para mapa
void Server::sendChannelMessage(const std::string& channel, const std::string& message, int indexClient) {
	std::cout << clients[indexClient].getNickName() << " sent a message to channel " << channel << ": " << message << std::endl;
	// Check if the indexClient is in the channel
	if (clients[indexClient].getCurrentChannel() != channel) {
		std::cout << "Client " << clients[indexClient].getNickName() << " is not in channel " << channel << std::endl;
		return;
	}
	// Find the channel
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it) {
		if (it->getName() == channel) {
			//TODO: Aqui deviamos ter um vetor com os clientes do canal
			std::vector<Client>::iterator it2;
			for (it2 = clients.begin(); it2 != clients.end(); ++it2) {
				if (it2->getCurrentChannel() == channel) {
					write(it2->getClientSocket(), message.c_str(), message.size());
				}
			}
			break;
		}
	}
}

void Server::listChannelUsers(const std::string& channelName) {
	std::cout << "List of users in channel " << channelName << ":" << std::endl;
	// Find the channel
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it) {
		if (it->getName() == channelName) {
			it->listMembers();
			break;
		}
	}
}	

void Server::changeChannelTopic(const std::string& channel, const std::string& topic, int indexClient) {
	// Check if the indexClient is in the channel
	if (clients[indexClient].getCurrentChannel() != channel) {
		std::cout << "Client " << clients[indexClient].getNickName() << " is not in channel " << channel << std::endl;
		return;
	}
	// Find the channel
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it) {
		if (it->getName() == channel) {
			// Check if the client is an operator of the channel
			if (it->isOperator(&clients[indexClient]) == false)
				std::cout << "Client " << clients[indexClient].getNickName() << " is not an operator of channel " << channel << std::endl;
			else {
				it->setTopic(topic);
				std::cout << clients[indexClient].getNickName() << " changed the topic of channel " << channel << " to " << topic << std::endl;
			}
			break;
		}
	}
}

void Server::viewChannelTopic(const std::string& channel, int indexClient) {
	// Find the channel
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it) {
		if (it->getName() == channel) {
			std::cout << "Topic of channel " << channel << ": " << it->getTopic() << std::endl;
			// Send the topic to the client
			write(clients[indexClient].getClientSocket(), it->getTopic().c_str(), it->getTopic().size());
			break;
		}
	}
}