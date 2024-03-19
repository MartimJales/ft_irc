#include "Client.hpp"

Client::Client(int socket) : clientSocket(socket) {
	// Initialize the client's nickname and username.
}

Client::~Client() {
	// Close the client's socket.
}

void Client::processInput() {
	std::cout << "Processing input from client." << std::endl;
	std::cout << "clientSocket: " << clientSocket << std::endl;
	// Read input from the client's socket.
	// Parse the input and take appropriate action.
	// Send output to the client's socket.
}

int Client::getClientSocket() {
	return clientSocket;
}

void Client::setNickname(std::string nickname) {
	this->nickname = nickname;
}
