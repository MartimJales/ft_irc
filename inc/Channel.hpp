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
	void handleModes(char *tokens, Client *client);
	void setInviteOnly(char *tokens, Client *client);
	void removeInviteOnly(char *tokens, Client *client);
	void setTopicProtected(char *tokens, Client *client);
	void removeTopicProtected(char *tokens, Client *client);
	void setKeyProtected(char *tokens, Client *client);
	void removeKeyProtected(char *tokens, Client *client);
	void setUsersLimit(char *tokens, Client *client);
	void removeUsersLimit(char *tokens, Client *client);
	void addOperatorPremission(char *tokens, Client *client);
	void removeOperatorPremission(char *tokens, Client *client);
	// TODO: Isto devia estar privado, coloquei aqui só por causa do operator<<
	bool	inviteOnly;
	bool	topicProtected;
	bool	keyProtected;
	int		usersLimit;
    std::vector<Client*> operators;
private:
    std::string name;
    std::string topic;
	std::string key;
    std::vector<Client*> members;
    std::vector<Client*> banned;

	// Map of modes and functions
    std::map<std::string, void (Channel::*)(char *, Client *)> modes;

};

//Overload the operator << to print the channel name
std::ostream& operator<<(std::ostream& os, const Channel& channel);

#endif // CHANNEL_HPP
