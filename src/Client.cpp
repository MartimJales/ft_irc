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

std::string Client::getNickName(void){
	return nickname;
}


void Client::setUsername(std::string username) {
	this->username = username;
}

std::string Client::getUsername(void){
	return username;
}

void Client::setCurrentChannel(std::string channel) {
	this->currentChannel = channel;
}

std::string Client::getCurrentChannel(void){
	return currentChannel;
}

