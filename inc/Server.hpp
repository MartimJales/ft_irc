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
#include <string>
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


    //Commands
    void listClients(void);
    void listChannels(void);
    void listChannelUsers(const std::string& channelName);
    void changeUserNickName(const std::string& nickname, int indexClient);
    void changeUserName(const std::string& username, int indexClient);
    void joinChannel(const std::string& channelName, int indexClient);
    void sendDirectMessage(const std::string& nickname, const std::string& message, int indexClient);
    void sendChannelMessage(const std::string& channel, const std::string& message, int indexClient);
    void changeChannelTopic(const std::string& channel, const std::string& topic, int indexClient);
    void viewChannelTopic(const std::string& channel, int indexClient);
};

#endif // SERVER_HPP
