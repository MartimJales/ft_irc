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
#include <map>
#include "Client.hpp"
#include "Channel.hpp"
#include "Replies.hpp"

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();
    void run();
	Client *getClient(std::string nickname);

private:
    int listenSocket;
    std::string password;
    std::vector<Client> clients;
	std::vector<pollfd> fds;
	std::vector<Channel> channels;
    // Map of commands and functions
    std::map<std::string, void (Server::*)(char *, int)> commands;
    // Function to create commands map
    void createCommandsMap(void);


    void processInstruction(int clientSocket, int index);
	void addClient(void);
	void removeClient(int clientSocket, int index);
	void executeCommand(const std::string& command, int indexClient);

    // Commands needed by the subject
    void checkPassword(char *tokens, int clientSocket);
    void changeUserNickName(char *tokens, int indexClient);
    void changeUserName(char *tokens, int indexClient);
    void joinChannel(char *tokens, int indexClient);
    void sendPrivateMessage(char *tokens, int indexClient);
    void kickUser(char *tokens, int indexClient);
    void inviteUser(char *tokens, int indexClient);
    void topicChannel(char *tokens, int indexClient);
    void channelModes(char *tokens, int indexClient);
    void quit(char *tokens, int indexClient);

    //Commands to debug or auxiliary
    void sendDirectMessage(const std::string& nickname, const std::string& message, int indexClient); // Send a message to a specific client
    void sendChannelMessage(const std::string& channel, const std::string& message, int indexClient); // Send a message to a specific channel
    void listClients(char *tokens, int indexClient);
    void listChannels(char *tokens, int indexClient);
    void listChannelUsers(const std::string& channelName);
    void changeChannelTopic(const std::string& channel, const std::string& topic, int indexClient);
    void viewChannelTopic(const std::string& channel, int indexClient);

    //Auxiliary functions
    void sendToClient(int clientSocket, const std::string& message);
	void debugOnly(char *tokens, int indexClient);
};

#endif // SERVER_HPP
