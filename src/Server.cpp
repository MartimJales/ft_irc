#include "Server.hpp"

// Constructor
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
	createCommandsMap();
	std::cout << "Server started on port " << port << std::endl;
}

// Destructor
Server::~Server() {
    // Close the listening socket and all client sockets.
    close(listenSocket);
    // Close all client sockets.
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		close(it->getClientSocket());
	}
}

// Run the server
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

// Proces the instruction from the client
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

// Execute the command from the client
void Server::executeCommand(const std::string& command, int indexClient){

	char* command_cstr = new char[command.size() + 1];
    std::strcpy(command_cstr, command.c_str());
	// Split the command to an array
	char *tokens = std::strtok(command_cstr, " \n\r");


	// While tokens is not NULL
	while (tokens != NULL)
	{
		// Run the command corresponding to the command
		// Check if the command exists
		if (commands.find(tokens) == commands.end())
		{
			std::cout << "Command " << tokens << " does not exist." << std::endl;
			tokens = std::strtok(NULL, " \n\r");
			continue;
		}
		// Run the command
		(this->*commands[tokens])(tokens, indexClient);
		// Tokens after method
		tokens = std::strtok(NULL, " \n\r");
	}

	// // Run the command corresponding to the command
	// // Check if the command exists
	// while (commands.find(tokens) == commands.end())
	// {
	// 	std::cout << "Command " << tokens << " does not exist." << std::endl;
	// 	tokens = std::strtok(NULL, " \n\r");
	// }
	// // TODO: How to send a welcome message to the client.
	// // std::string message = ":irc_server 001 jales :Welcome to the Internet Relay Network jales!jales@localhost\r\n";
	// // write(clients[indexClient].getClientSocket(), message.c_str(), message.size());
	// // std::cout << "Enviou mensagem" << std::endl;

	// std::cout << "Tokens before method call: " << tokens << std::endl;
	// (this->*commands[tokens])(tokens, indexClient);
	// // Tokens after method
	// std::cout << "Tokens after method call: " << tokens << std::endl;
}

// Check if the password is correct
// PASS <password>
void Server::checkPassword(char *tokens, int indexClient)
{
	std::cout << "Client " << indexClient << " sent a password." << std::endl;
	// Get the password
	tokens = std::strtok(NULL, " \n\r");

	// Tokens in method checkPassword
	std::cout << "Tokens in method: " << tokens << std::endl;
	// Check if the password is correct
	if (strncmp(tokens, password.c_str(), password.size()) != 0)
	{
		std::cout << "Client " << indexClient << " sent an incorrect password." << tokens << std::endl;
		sendToClient(indexClient, "Incorrect password");
		close(indexClient);
		return ;
	}
	// sendToClient(clients[indexClient].getClientSocket(), "Correct password");
	return ;
}

// Change the nickname of the client
// NICK <nickname>
void Server::changeUserNickName(char *tokens, int indexClient) {
	tokens = std::strtok(NULL, " \n\r");
	std::string nickname(tokens);
	std::cout << "Client " << indexClient << " changed nickname to " << nickname << std::endl;
	clients[indexClient].setNickname(nickname);
}

// Change the username of the client
// USER <username> <mode> <unused> <realname>
void Server::changeUserName(char *tokens, int indexClient) {
	tokens = std::strtok(NULL, " \n\r");
	std::string username(tokens);
	std::cout << "Client " << indexClient << " changed username to " << username << std::endl;
	clients[indexClient].setUsername(username);
}

// Join a channel
// JOIN #<channel>
// TODO: We need to see the situation of the # before the name of the channel, i think it is better to keep it forever
// TODO: Regarding te mode of the channel, sometimes we need to send the password of the channel when trying to join it
void Server::joinChannel(char *tokens, int indexClient) {
	tokens = std::strtok(NULL, " \n\r");
	std::string channelName(tokens);
	std::cout << "Client " << indexClient << " joined channel " << channelName << std::endl;

	// Check if the channel exists
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it) {
		if (it->getName() == channelName) {
			//TODO: Temos que retirar o cliente do canal anterior
			it->join(&clients[indexClient]);
			clients[indexClient].setCurrentChannel(channelName);
			break;
		}
	}
	// If the channel does not exist, create it
	if (it == channels.end()) {
		Channel newChannel(channelName);
		newChannel.join(&clients[indexClient]);
		channels.push_back(newChannel);
		clients[indexClient].setCurrentChannel(channelName);
	}
}

// Send a direct message to a user or a channel
// PRIVMSG <nickname> <message>
// PRIVMSG #<channel> <message>
void Server::sendPrivateMessage(char *tokens, int indexClient) {
	tokens = std::strtok(NULL, " \n");
	std::string nickname(tokens);
	tokens = std::strtok(NULL, "\n");
	std::string message(tokens);


	// Check if the message is to a user or a channel
	if (nickname[0] == '#') {
		// TODO: Here we use the name of the channel with the #
		std::cout << "Client " << indexClient << " sent a message to channel " << nickname << ": " << message << std::endl;
		sendChannelMessage(nickname, message, indexClient);
	}
	else {
		std::cout << "Client " << indexClient << " sent a direct message to " << nickname << ": " << message << std::endl;
		sendDirectMessage(nickname, message, indexClient);
	}
	std::string nickDemo("DEMO");
	std::string messageDemo("TESTE");
	std::string reply(RPL_PRIVMSG(nickDemo, messageDemo, nickname, message));
	write(clients[indexClient].getClientSocket(), reply.c_str(), reply.size());
}

// Kick a user from a channel
// KICK <server> #<channel> :<nickname>
// TODO: We need to review all this logic
// void Server::kickUser(char *tokens, int indexClient) {
// 	tokens = std::strtok(NULL, " \n");
// 	std::string server(tokens);
// 	tokens = std::strtok(NULL, " \n");
// 	std::string channel(tokens);
// 	// Ignore the ":"
// 	tokens = std::strtok(NULL, ":");
// 	tokens = std::strtok(NULL, " \n");
// 	std::string nickname(tokens);
// 	std::cout << "Client " << indexClient << " kicked " << nickname << " from channel " << channel << std::endl;
// 	std::vector<Channel>::iterator it;
// 	for (it = channels.begin(); it != channels.end(); ++it) {
// 		if (it->getName() == channel) {
// 			// Check if the client is an operator of the channel
// 			if (it->isOperator(&clients[indexClient]) == false)
// 				std::cout << "Client " << clients[indexClient].getNickName() << " is not an operator of channel " << channel << std::endl;
// 			else {
// 				// Find the client with the nickname
// 				std::vector<Client>::iterator it2;
// 				for (it2 = clients.begin(); it2 != clients.end(); ++it2) {
// 					if (it2->getNickName() == nickname) {
// 						// it->removeMember(&(*it2));
// 						break;
// 					}
// 				}
// 			}
// 			break;
// 		}
// 	}
// }

// Invite a user to a channel
// INVITE <nickname> #<channel>
void Server::inviteUser(char *tokens, int indexClient) {
	tokens = std::strtok(NULL, " \n");
	std::string nickname(tokens);
	tokens = std::strtok(NULL, "\n");
	std::string channel(tokens);
	std::cout << "Client " << indexClient << " invited " << nickname << " to channel " << channel << std::endl;
	// Find the client with the nickname
	std::vector<Client>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it) {
		if (it->getNickName() == nickname) {
			// TODO: We need to save the invited clients in the channel
			write(it->getClientSocket(), "Invite to channel ", 17);
			write(it->getClientSocket(), channel.c_str(), channel.size());
			break;
		}
	}
}

// Topic Command
// TOPIC #<channel> [:<topic>]
void Server::topicChannel(char *tokens, int indexClient) {
	tokens = std::strtok(NULL, " \n");
	std::string channel(tokens);
	tokens = std::strtok(NULL, " \n");
	// If tokens is NULL, we are just viewing the topic
	if (tokens == NULL)
	{
		viewChannelTopic(channel, indexClient);
	}
	else {
		std::string topic(tokens);
		changeChannelTopic(channel, topic, indexClient);
	}
}

// Channel Modes
// TODO: We need to implement this
void Server::channelModes(char *tokens, int indexClient) {
	// Show the tokens
	tokens = std::strtok(NULL, " \n");
	std::string channel(tokens);
	std::cout << "Tokens in channelModes: " << tokens << std::endl;
	// Find the currrent channel of the client
	std::string clientChannel = clients[indexClient].getCurrentChannel();
	if (clientChannel != channel) {
		std::cout << "Client " << clients[indexClient].getNickName() << " is not in channel " << channel << std::endl;
		return;
	}
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it) {
		if (it->getName() == channel) {
			std::cout << "Client " << indexClient << " sent a mode to channel " << channel << std::endl;
			if (it->isOperator(&clients[indexClient]))
				it->handleModes(tokens, &clients[indexClient]);
			else
				std::cout << "Client " << clients[indexClient].getNickName() << " is not an operator of channel " << channel << std::endl;
			break;
		}
	}
}

// Quit
// QUIT [:<message>]
void Server::quit(char *tokens, int indexClient) {
	tokens = std::strtok(NULL, " \n");
	std::string message(tokens);
	std::cout << "Client " << indexClient << " disconnected." << std::endl;
	if (message.size() > 0)
		std::cout << "Message: " << message << std::endl;
	removeClient(clients[indexClient].getClientSocket(), indexClient);
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
	// sendToClient(clientSocket, "Hello World");
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

//list clients compatible with map
void Server::listClients(char *tokens, int indexClient) {
	(void)tokens;
	(void)indexClient;
	std::cout << "List of clients:" << std::endl;
	std::vector<Client>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it) {
		std::cout << "Nickname: " << it->getNickName() << std::endl;
		std::cout << "Username: " << it->getUsername() << std::endl;
	}
}

//list channels compatible with map
void Server::listChannels(char *tokens, int indexClient) {
	(void)tokens;
	(void)indexClient;
	std::cout << "List of channels:" << std::endl;
	std::vector<Channel>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it) {
		std::cout << *it << std::endl;
	}
}

void Server::sendDirectMessage(const std::string& nickname, const std::string& message, int indexClient) {
	std::cout << clients[indexClient].getNickName() << " sent a direct message to " << nickname << ": " << message << std::endl;
	// Find the client with the nickname
	std::vector<Client>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it) {
		if (it->getNickName() == nickname) {
			// Send the message to the client
			std::string nickDemo("diogo");
			std::string messageDemo("dcordovi");
			std::string reply(RPL_PRIVMSG(nickDemo, messageDemo, nickname, message));
			sendToClient(it->getClientSocket(), reply);
			// write(it->getClientSocket(), message.c_str(), message.size());
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

void Server::debugOnly(char *tokens, int indexClient){
	(void)tokens;
	(void)indexClient;
	std::cout << "If you see this message you need to update createCommandsMap" << std::endl;
}


void Server::createCommandsMap(void){
	commands["PASS"] = &Server::checkPassword;
	commands["NICK"] = &Server::changeUserNickName;
	commands["USER"] = &Server::changeUserName;
	commands["JOIN"] = &Server::joinChannel;
	commands["PRIVMSG"] = &Server::sendPrivateMessage;
	commands["KICK"] = &Server::debugOnly;
	commands["INVITE"] = &Server::debugOnly;
	commands["TOPIC"] = &Server::debugOnly;
	commands["MODE"] = &Server::channelModes;
	commands["QUIT"] = &Server::debugOnly;
	commands["LIST"] = &Server::listClients;
	commands["LIST_C"] = &Server::listChannels;
	// commands["CHANNEL_USERS"] = &Server::listChannelUsers;
}


void Server::sendToClient(int clientSocket, const std::string& message) {
	int n = write(clientSocket, message.c_str(), message.size());
	if (n < 0) {
		throw std::runtime_error("Error writing to socket");
	}
}

Client * Server::getClient(std::string nickname)
{
    std::vector<Client>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) {
        if (it->getNickName() == nickname) {
            return &(*it); // Return pointer to the Client object
        }
    }
    return NULL; // Return nullptr if client not found
}
