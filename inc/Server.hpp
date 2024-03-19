#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <algorithm>
#include "Client.hpp"
#include "Channel.hpp"

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();
    void run();

private:
    int listenSocket;
    std::string password;
    std::vector<Client> clients;
	std::vector<pollfd> fds;
	std::vector<Channel> channels;
    void processInstruction(int clientSocket, int index);
	void addClient(void);
	void removeClient(int clientSocket, int index);	
	bool checkPassword(int clientSocket);
	void executeCommand(const std::string& command, int indexClient);
};

#endif // SERVER_HPP
