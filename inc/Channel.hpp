#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <iostream>
#include <cstdlib>
#include "Client.hpp"

class Channel {
public:
    Channel(const std::string& name);
    ~Channel();

    // Getters
    std::string getName() const;
    std::string getTopic() const;

    //Setters
    void setName(const std::string& name);
    void setTopic(const std::string& topic);

    //Methods
    void addMember(Client* member);
    void removeMember(Client* member);
    void addOperator(Client* op);
    void removeOperator(Client* op);
    void listMembers();
    bool isOperator(Client* client);

	// Modes
	void handleModes(char *tokens, int clientSocket);
	void setInviteOnly(char *tokens, int clientSocket);
	void removeInviteOnly(char *tokens, int clientSocket);
	void setTopicProtected(char *tokens, int clientSocket);
	void removeTopicProtected(char *tokens, int clientSocket);
	void setKeyProtected(char *tokens, int clientSocket);
	void removeKeyProtected(char *tokens, int clientSocket);
	void setUsersLimit(char *tokens, int clientSocket);
	void removeUsersLimit(char *tokens, int clientSocket);
	// TODO: Isto devia estar privado, coloquei aqui só por causa do operator<<
	bool	inviteOnly;
	bool	topicProtected;
	bool	keyProtected;
	int		usersLimit;
private:
    std::string name;
    std::string topic;
	std::string key;
    std::vector<Client*> members;
    std::vector<Client*> banned;
    std::vector<Client*> operators;

	// Map of modes and functions
    std::map<std::string, void (Channel::*)(char *, int)> modes;

};

//Overload the operator << to print the channel name
std::ostream& operator<<(std::ostream& os, const Channel& channel);

#endif // CHANNEL_HPP
